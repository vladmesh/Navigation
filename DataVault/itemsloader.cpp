#include "itemsloader.h"

#include <algorithm>

QDir ItemsLoader::dirpath = QDir();
ItemsStorage ItemsLoader::vault;
QStringList ItemsLoader::loadErrors;
ItemsLoader &ItemsLoader::instance()
{
    static ItemsLoader instance;
    return instance;
}

ItemsLoader::ItemsLoader(QObject *parent)
    : QObject{parent}
{}

QVector<QPointer<FixedItem>> ItemsLoader::readFromJSON(QFile* file, QStringList* errors)
{
    QVector<QPointer<FixedItem>> res;
    loadErrors.clear();
    struct ReportErrors {
        QStringList* out;
        ~ReportErrors()
        {
            if (out)
                *out = loadErrors;
        }
    } report{errors};
    // Ensure the file is open and ready to be read
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Couldn't open the file.");
        loadErrors << QString("cannot open %1").arg(file->fileName());
        return res;
    }

    // Read the entire content of the file into a QJsonDocument
    QByteArray fileContent = file->readAll();
    file->close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);
    if (!jsonDoc.isObject()) {
        qWarning("Invalid JSON format.");
        loadErrors << "invalid JSON format";
        return res;
    }

    QJsonObject jsonObj = jsonDoc.object();

    QVector<QJsonValue> sortedArrayOfJson;
    sortedArrayOfJson.reserve(jsonObj.size());
    for (auto i:jsonObj) {
        sortedArrayOfJson.push_back(i);
    }
    // убираем записи без имени
    sortedArrayOfJson.removeIf([](const QJsonValue& v) { return v["name"].toString().isNull(); });
    // объект создаётся после того, к кому он привязан: порядок по creationPriority
    std::stable_sort(sortedArrayOfJson.begin(), sortedArrayOfJson.end(), [](const QJsonValue& a, const QJsonValue& b) {
        return a["creationPriority"].toInt() < b["creationPriority"].toInt();
    });

    for (const QJsonValue& value : sortedArrayOfJson) {
        FixedItem* item = createItemFromJson(value);
        if (item == nullptr)
            continue;
        res.append(item);
        vault.SaveItem(item);
    }

    vault.sortItemsByCalcPriority();
    vault.setItemForCalculation(res);


    vault.ItemsVault.clear();
    return res;
}

FixedItem* ItemsLoader::createItemFromJson(QJsonValue jsonObject)
{
    //если оно выглядит как костыль, и пахнет как костыль, это еще не значит, что это костыль
    static bool ifStreamerNeedsEndBuoy = false;
    static Streamer* streamerNeedsEndBuoy;
    static QString endBuoyNameForStreamer;

    static Fabric fabric; //объект создается только для того, чтобы вызывать его методы

    FixedItem* item = nullptr;

    QJsonObject jsonItem = jsonObject.toObject();
    QString itemType = jsonItem["type"].toString();
    QString name = jsonItem["name"].toString();
    int creationPriority = jsonItem["creationPriority"].toInt(-1);
    //общие параметры для всех объектов


    if (itemType == "Fixed") {
        FixedItemInfo info(jsonItem["x"].toDouble(0),
                           jsonItem["y"].toDouble(0),jsonItem["z"].toDouble(0),name);
        item = fabric.CreateItem(info, false); //создаем без подключения, чтобы не вызывать ConnectionCreator
        vault.SaveItem(item);
    } else {
        //общие параметры для всех буксируемых объектов
        QJsonObject jsonTowedItem = jsonItem["TowedInfo"].toObject();
        QString towingPointName = jsonTowedItem["Towing point"].toString();
        double absoluteLayback = jsonTowedItem["absoluteLayback"].toDouble();
        FixedItem* towingPoint = vault.getItem(towingPointName);
        if (towingPoint == nullptr) {
            qWarning() << "ItemsLoader: towing point" << towingPointName << "not found for" << name << ", item skipped";
            loadErrors << QString("%1: towing point \"%2\" not found").arg(name, towingPointName);
            return nullptr;
        }

        if (itemType == "Towed") {
            TowedItemInfo info(towingPoint, absoluteLayback, 90, name);
            TowedItem* towedItem = fabric.CreateItem(info,false);
            item = towedItem;
        } else if (itemType == "Source") {
            TowedItemInfo info(towingPoint, absoluteLayback, 90, name);
            QJsonObject jsonSourceItem = jsonTowedItem["source"].toObject();
            float sourceDepth = jsonSourceItem["sourceDepth"].toDouble();
            float AnthenaHeight = jsonSourceItem["AnthenaHeight"].toDouble();
            Source* sourceItem = fabric.CreateItem(SourceInfo(info, AnthenaHeight, sourceDepth), false);
            item = sourceItem;
        } else if (itemType == "Buoy") {
            QJsonObject jsonBuoyItem = jsonTowedItem["buoy"].toObject();
            TowedItemInfo info(towingPoint, absoluteLayback, 90, name);
            BuoyInfo infoB(info, jsonBuoyItem["AnthenaHeight"].toDouble(), jsonBuoyItem["towingDepth"].toDouble());
            Buoy* buoyItem  = fabric.CreateItem(infoB, false);

            item = buoyItem;
            if (buoyItem && ifStreamerNeedsEndBuoy && buoyItem->name == endBuoyNameForStreamer) {
                streamerNeedsEndBuoy->setEndBuoy(buoyItem);
            }
        } else if (itemType == "Streamer") {
            QJsonObject jsonStreamerItem = jsonTowedItem["Streamer"].toObject();

            Streamer* streamerItem =
                new Streamer(name, towingPoint, NULL, absoluteLayback, jsonStreamerItem["NumChanels"].toInt(0),
                             jsonStreamerItem["Step between Channels"].toString());
            item = streamerItem;
            if (jsonStreamerItem["hasEndBuoy"].toBool()) {
                ifStreamerNeedsEndBuoy = true;
                streamerNeedsEndBuoy = streamerItem;
                endBuoyNameForStreamer = jsonStreamerItem["endBuoyName"].toString();
            }
        }
    }
    if (item != nullptr) {
        //общие параметры для соединения
        vault.SaveItem(item);
        item->name = jsonItem["name"].toString();
        item->itemType = itemType;
        item->hasConnection = jsonItem["hasConnection"].toBool();
        if (item->hasConnection) {
            Connection* con = jsonToConnection(jsonItem["connection"].toObject());
            if (con != nullptr) {
                fabric.bindItemConnection(item, con);
                con->ui->show();
            } else {
                qWarning() << "ItemsLoader: no IP or COMport in connection of" << name;
                loadErrors << QString("%1: connection has neither IP nor COMport, loaded without connection").arg(name);
                item->hasConnection = false;
            }
        }
        item->creationPriority = creationPriority;
    } else {
        loadErrors << QString("%1: unknown or broken item (type \"%2\")").arg(name, itemType);
    }
    vault.setItemForCalculation(vault.ItemsVault);

    return item;

}

Connection* ItemsLoader::jsonToConnection(const QJsonObject &obj) {
    Connection* conn = nullptr;

    QString filename = obj["filename"].toString();


    filename = dirpath.absolutePath() + "/" + filename;
    if (!filename.endsWith(".nmea", Qt::CaseInsensitive)) {
        filename += ".nmea";
    }

    if (obj.contains("IP")) {
        QString IP = obj["IP"].toString().split(' ')[0];
        int port = obj["port"].toInt();

        Connection_Net* netConn = new Connection_Net(
            IP, port, filename);
        netConn->setFilename(filename);
        conn = netConn;
    } else if (obj.contains("COMport")) {
        // Connection_com
        int BaudRate = obj["BaudRate"].toInt();
        QString COMPort = obj["COMport"].toString();
        Connection_com* comConn = new Connection_com
            (COMPort, BaudRate, filename);
        conn = comConn;
    }

    return conn;
}

QJsonObject ItemsLoader::writeToJSON(QVector<QPointer<FixedItem>> ItemsVector)
{
    setCreationPriority(ItemsVector);
    QJsonObject obj;
    QJsonObject jsonFixed; //for (int i = ItemsVector.size() - 1; i >= 0; i--) {
    for (int i = 0; i < ItemsVector.size(); i++) {
        FixedItem* item = ItemsVector.at(i);
        jsonFixed["name"] = item->name;

        jsonFixed["creationPriority"] = item->creationPriority;
        jsonFixed["type"] = item->itemType;

        if (item->connection != nullptr) {
            jsonFixed["hasConnection"] = true;
        }


        if (item->hasConnection || item->connection != nullptr) {
            jsonFixed["connection"] = connectionToJson(item->connection);
        }

        if (item->itemType == "Fixed") {
            jsonFixed["x"] = item->x;
            jsonFixed["y"] = item->y;
            jsonFixed["z"] = item->z;
        } else if (item->itemType == "Towed" || item->itemType =="Buoy"
                   || item->itemType == "Streamer"|| item->itemType == "Source") {
            QJsonObject jsonTowed;
            TowedItem* itemTowed = dynamic_cast<TowedItem*>(item);
            jsonTowed["Towing point"] = itemTowed->towingPoint->name;

            jsonTowed["absoluteLayback"] = itemTowed->absoluteLayback;

            if (item->itemType == "Buoy") {
                QJsonObject jsonBuoy;
                jsonBuoy["AnthenaHeight"] = dynamic_cast<Buoy*>(item)->AnthenaHeight;
                jsonBuoy["towingDepth"] = dynamic_cast<Buoy*>(item)->towingDepth;
                jsonTowed["buoy"] = jsonBuoy;
            } else if (item->itemType == "Streamer") {
                QJsonObject jsonStreamer;
                QJsonArray chans;
                uint numChans = dynamic_cast<Streamer*>(item)->getChanCount();
                if (!dynamic_cast<Streamer*>(item)->singleChannelType.isEmpty()) {
                    jsonStreamer["singleChannelType"] = dynamic_cast<Streamer*>(item)->singleChannelType;
                }
                if (dynamic_cast<Streamer*>(item)->endBuoy != nullptr) {
                    jsonStreamer["hasEndBuoy"] = true;
                    jsonStreamer["endBuoyName"] = dynamic_cast<Streamer*>(item)->endBuoy->name;
                } else {
                    jsonStreamer["hasEndBuoy"] = false;
                }

                jsonStreamer["NumChanels"] = (int)numChans;
                jsonStreamer["Step between Channels"] = dynamic_cast<Streamer*>(item)->channelStep;

                jsonTowed["Streamer"] = jsonStreamer;
                jsonStreamer = QJsonObject();
            } else if (item->itemType == "Source") {
                QJsonObject jsonSource;
                jsonSource["AnthenaHeight"] = dynamic_cast<Source*>(item)->AnthenaHeight;
                jsonSource["sourceDepth"] = dynamic_cast<Source*>(item)->sourceDepth;
                jsonTowed["source"] = jsonSource;
            }
            jsonFixed["TowedInfo"] = jsonTowed;
            jsonTowed = QJsonObject();


        }
        obj[item->name] = jsonFixed;
        jsonFixed = QJsonObject();
    }

    return obj;
}


QJsonObject ItemsLoader::connectionToJson(Connection *conn) {
    QJsonObject obj;
    obj["filename"] = conn->filename;

    if (QString(conn->metaObject()->className()) == "Connection_Net") {
        obj["IP"] = dynamic_cast<Connection_Net*>(conn)->getIP();
        obj["port"] = dynamic_cast<Connection_Net*>(conn)->getPort();
    } else if (QString(conn->metaObject()->className()) == "Connection_com") {
        obj["COMport"] = dynamic_cast<Connection_com*>(conn)->getComPort();
        obj["BaudRate"] = dynamic_cast<Connection_com*>(conn)->getBaudRate();
    }
    obj["type"] = QString(conn->metaObject()->className());
    return obj;
}

bool ItemsLoader::saveJsonObjectToFile(QJsonObject &jsonObject, QString &filePath) {
    // Convert QJsonObject to QJsonDocument
    QJsonDocument jsonDoc(jsonObject);

    // Open the file in write mode
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << file.errorString();
        return false;
    }

    // Write the JSON document to the file
    file.write(jsonDoc.toJson());
    file.close(); // Close the file

    return true;
}


void ItemsLoader::setCreationPriority(QVector<QPointer<FixedItem>> &ItemsVault)
{
    //я не уверен, должно ли мне быть стыдно за это или наоборот, можно радоваться
    ItemsStorage::setItemsCreationIndex(ItemsVault);
}
