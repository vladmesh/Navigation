#include "p190_creator.h"



QDir P190_creator::path = QDir();

P190_creator::P190_creator(QObject *parent)
    : QObject{parent}
{
    initializeHeaderFile();
}

P190_creator::~P190_creator()
{
    outputFile->close();
    outputStream->flush();
}

QStringList P190_creator::createShotBlock()
{
    if (-1 == FFID) return QStringList();
    calcItemsCount();
    QStringList res;
    QStringList mainBlock = createMainInfoBlock(this->FFID);
    QStringList streamerBlock = createStreamerBlock();

    // for (QString l: mainBlock) {
    //     // qDebug() << l;
    // }
    // for (QString l: streamerBlock) {
    //     // qDebug() << l;
    // }

    res.append(mainBlock);
    res.append(streamerBlock);
    writeToFile(res);
    return res;
}




QStringList P190_creator::createHeader(int dayNumber) {
    // initializeHeaderFile(); // Проверяем/копируем файл при запуске
    QString filePath = "./ModifiedSurveyInfo.txt";
    QFile file(filePath);
    QStringList header = initializeHeaderFile();

    // if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     QTextStream in(&file);

    //     // Читаем строки из файла и добавляем их в QStringList
    //     while (!in.atEnd()) {
    //         QString line = in.readLine();
    //         header << line;
    //     }

    //     file.close();
    // } else {
    //     // Обработка ошибки, если файл не удалось открыть
    //     qWarning("Cannot open file for reading: %s", qPrintable(file.errorString()));
    // }
    for (int i = 0; i < header.size(); i++) {
        if (header[i].contains("H0200 DATE OF SURVEY")) {
            header[i] = replaceDates(header[i]);
            header[i+1] = replaceDates(header[i+1]);
            break;
        }
    }

    return header;
}

QStringList P190_creator::createHeader() {
    // QString filePath = ":/Data/SurveyInfo.txt";
    // QFile file(filePath);
    QStringList header = initializeHeaderFile();

    // if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     QTextStream in(&file);

    //     // Читаем строки из файла и добавляем их в QStringList
    //     while (!in.atEnd()) {
    //         QString line = in.readLine();
    //         line = line.remove('"');
    //         header << line;
    //     }

    //     file.close();
    // } else {
    //     // Обработка ошибки, если файл не удалось открыть
    //     qWarning("Cannot open file for reading: %s", qPrintable(file.errorString()));
    // }
    for (int i = 0; i < header.size(); i++) {
        if (header[i].contains("H0200 DATE OF SURVEY")) {
            header[i] = replaceDates(header[i]);
            header[i+1] = replaceDates(header[i+1]);
            break;
        }
    }

    return header;
};



void P190_creator::createP190File() {
    if (headerIsWritten || dayOfSurvey == -1) {
        return;
    }
    QDir dir = path; //setted in MainWindow
    fileName = createFileName();
    outputFile = new QFile(dir.absolutePath() + "/" + fileName);
    outputFile->open(QIODevice::Append);
    QStringList list = createHeader();
    for (QString l:list) {
        outputFile->write(QByteArray(l.toUtf8()));
        outputFile->write("\n");
    }

    headerAnalyzer.analizeVault();
    outputFile->close();
}

void P190_creator::createP190File(int dayNumber)
{
    if (!headerIsWritten) {
        this->dayOfSurvey = dayNumber;
        createP190File();
        headerIsWritten = true;
    } else {
        return;
    }
}




void P190_creator::writeToFile(QStringList data) {
    if (outputFile->open(QIODevice::Append)){
        for (QString str: data) {
            if (str.isEmpty() || str.length() < 20) continue;
            outputFile->write(QByteArray(str.toUtf8()) + "\n");
            // logmsg(str);
                // outputFile->write("\n");
        }
        outputFile->close();
    }
}

QStringList P190_creator::createStreamerBlock() {
    QStringList res;
    ///TODO: убрать костыли

    for (FixedItem* item: MyVault->ItemsVault) {
        if (QString(item->metaObject()->className()) == "Streamer") {
            Streamer* strm = dynamic_cast<Streamer*>(item);
            QString tmp = "R";
            for (uint i = 0; i < strm->getChanCount(); ++i) {
                if (i % 3 == 0) {
                    if (!tmp.isEmpty()) {
                        if (currentStreamerNumber >1) {
                            tmp += QString::number(strm->streamerID);
                        }
                        res.append(tmp);
                        tmp = "R";
                    } ///I dont fucking remember how does it work

                }
                tmp += strm->getChan(i+1)->getUTMPos(); // в массиве от 0 до 23
                //каналы от 1 до 24
            }

            if (!tmp.isEmpty()) {
                if (currentStreamerNumber >1) {
                    tmp += QString::number(strm->streamerID);
                }
                res.append(tmp);
            }
        }
    }
    return res;
}


QStringList P190_creator::createMainInfoBlock(int ffid) {
    currentBuoyNumber = 0;
    currentSourceNumber = 0;
    QStringList res;
    QString tmp;
    // int pointNumber = ffid;
    for (FixedItem* item: MyVault->ItemsVault) {
        if (QString(item->metaObject()->className()) == "Streamer") {
            continue;
        }

        tmp = createMainRow__new(item, ffid, currentBuoyNumber);
        if (QString(item->metaObject()->className()) == "Buoy") {
            currentBuoyNumber++;
        }
        // currentSourceNumber++;
        if (tmp.isEmpty()) continue;
        res.append(tmp);
    }

    return res;
}

QStringList P190_creator::createMainInfoBlock(int ffid, int sourceID)
{
    QStringList res = {};
    // int sourceFlag = -1;
    // for (FixedItem* item: MyVault->ItemsVault) {
    //     if (item->itemType == "Source") {
    //         if (dynamic_cast<Source*>(item)->curSourceNum == sourceID) {
    //             sourceFlag = sourceID;
    //             break;
    //         }
    //     }
    // }
    // if (sourceFlag+1) {
    if (true) {
        QString tmp;
        // int pointNumber = ffid;
        for (FixedItem* item: MyVault->ItemsVault) {
            if (QString(item->metaObject()->className()) == "Streamer") {
                continue;
            }
            // if (item->itemType == "Source") {
            //     if (dynamic_cast<Source*>(item)->curSourceNum != sourceID) {
            //         continue;
            //     }
            // }
            tmp = createMainRow__new(item, ffid, -1);
            // if (QString(item->metaObject()->className()) == "Buoy") {
            //     currentBuoyNumber++;
            // }
            // currentSourceNumber++;
            if (tmp.isEmpty()) continue;
            res.append(tmp);
        }
    }
    return res;
}

QString P190_creator::createMainRow__new(FixedItem *item, int pointNumber, int tailBuoy)
{
    QString res(80, ' ');
    QChar type = 'Z';
    QChar VesselID = '1';
    QChar SourceID = '1';
    QChar TailBuoyID = QString::number(tailBuoy).at(0);
    if (!TailBuoyID.isDigit()) TailBuoyID = ' ';
    QDateTime dt = item->lastGGAData.dateTime;
    if (dt.isValid()) dummyTimeForItemsWithNoTime = dt;
    if(QString(item->metaObject()->className()) == "FixedItem") {
        if (item->x == 0 && item->y == 0) {
            type = 'V';
            SourceID = ' ';
            TailBuoyID = ' ';
            if (item->lastGGAData.dateTime.isValid()) curDateTime = item->lastGGAData.dateTime;
        }  else {
            return "";
        }
    } else if(QString(item->metaObject()->className()) == "Buoy") {
        type = 'T';
        if (dynamic_cast<Buoy*>(item)->buoyID != -1) {
            TailBuoyID = QString::number(dynamic_cast<Buoy*>(item)->buoyID).at(0);
        } else {
            TailBuoyID = ' ';
        }

        // TailBuoyID = '1';
    } else if (QString(item->metaObject()->className()) == "Source") {
        type = 'S';
        // TailBuoyID = '1';
        if (dynamic_cast<Source*>(item)->sourceID != -1) {
            SourceID = QString::number(dynamic_cast<Source*>(item)->sourceID).at(0);
        } else {
            SourceID = ' ';
        }
        if (!SourceID.isDigit()) SourceID = ' ';
    } else {
        return "";
    }
    if (!dt.isValid()) {

        dt = dummyTimeForItemsWithNoTime;
    }

    res.replace(0, 1, type);
    res.replace(1, 12, lineName);
    res.replace(16, 1, VesselID);
    res.replace(17, 1, SourceID);
    res.replace(18, 1, TailBuoyID);
    // qDebug() <<"-"<<VesselID<<"-"<<SourceID<<"-"<<TailBuoyID<<"-"<<item->name;
    res.replace(19, 6, QString(6-QString::number(pointNumber).length(), ' ') + QString::number(pointNumber));
    res.replace(25, 21, QGeoCoordinate(item->latitude, item->longitude).
        toString(QGeoCoordinate::CoordinateFormat::DegreesWithHemisphere).remove("°").remove(' '));
    res.replace(46, 9, floatToQString(item->x_coor, 8,2));
    res.replace(55, 9, floatToQString(item->y_coor, 9,2));
    if (item->itemType == "Source") {
        res.replace(64, 5, floatToQString(dynamic_cast<Source*>(item)->sourceDepth, 5,1));
    } else if (item->hasConnection) {
        res.replace(64, 5, floatToQString(item->height, 5,1));
    } else {
        res.replace(64, 5, floatToQString(0.0, 5,1));
    }
    res.replace(70, 3, QString("%1").arg(dayOfSurvey, 3, 10, QChar('0')));
    res.replace(73, 2, QString("%1").arg(dt.time().hour(), 2, 10, QChar('0')));
    res.replace(75, 2, QString("%1").arg(dt.time().minute(), 2, 10, QChar('0')));
    res.replace(77, 2, QString("%1").arg(dt.time().second(), 2, 10, QChar('0')));
    return res;
}

void P190_creator::setPath(const QDir &newPath)
{
    path = newPath;
}

void P190_creator::setFFID(int newFFID)
{
    FFID = newFFID;
}

ItemsStorage *P190_creator::getMyVault() const
{
    return MyVault;
}

void P190_creator::setMyVault(ItemsStorage *newMyVault)
{
    MyVault = newMyVault;
    headerAnalyzer.Vault = MyVault;///not checked 11.02
}

void P190_creator::setDayOfSurvey(int newDayOfSurvey)
{
    dayOfSurvey = newDayOfSurvey;
}

void P190_creator::calcItemsCount()
{
    currentBuoyNumber = 0;
    currentSourceNumber = 0;
    currentStreamerNumber = 0;
    for (QPointer<FixedItem> p: MyVault->ItemsVault) {
        if (p->itemType == "Buoy") {
            currentBuoyNumber++;
        }
        if (p->itemType == "Source") {
            currentSourceNumber++;
        }
        if (p->itemType == "Streamer") {
            currentStreamerNumber++;
        }
    }
}

void P190_creator::setHeaderIsWritten(bool newHeaderIsWritten)
{
    headerIsWritten = newHeaderIsWritten;
}

QStringList P190_creator::initializeHeaderFile()
{
    qDebug() << __FUNCTION__;
    QString filePath = QCoreApplication::applicationDirPath() + "/ModifiedSurveyInfo.txt";
    QFile localFile(filePath);

    // Если файла нет, создаём его из ресурсов
    if (!localFile.exists()) {
        QFile resourceFile(":/Data/SurveyInfo.txt");
        if (!resourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open resource file:" << resourceFile.errorString();
            return QStringList(); // Возвращаем пустой список в случае ошибки
        }

        if (!localFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to create local file:" << localFile.errorString();
            return QStringList(); // Возвращаем пустой список в случае ошибки
        }

        QTextStream in(&resourceFile);
        QTextStream out(&localFile);
        out << in.readAll(); // Копируем содержимое ресурса в локальный файл

        resourceFile.close();
        localFile.close();
    }

    // Читаем содержимое локального файла в QStringList
    if (!localFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open local file for reading:" << localFile.errorString();
        return QStringList(); // Возвращаем пустой список в случае ошибки
    }

    QStringList lines;
    QTextStream stream(&localFile);
    while (!stream.atEnd()) {
        lines << stream.readLine();
    }

    localFile.close();
    return lines;
}

void P190_creator::setItemStoragePtr(ItemsStorage *Vault)
{
    this->MyVault = Vault;
}

void P190_creator::setLineName(const QString &newLineName)
{
    lineName = newLineName;
}

QString P190_creator::createFileName()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString fileName = QString("%1_%2_%3_%4.p190")
                           .arg(currentDateTime.date().day(),2, 10, QChar('0'))
                           .arg(currentDateTime.date().month(),2, 10, QChar('0'))
                           .arg(currentDateTime.time().hour())
                           .arg(currentDateTime.time().minute(), 2, 10, QChar('0'));  // Добавляем ведущий ноль для минут
    this->fileName = fileName;
    return fileName;
}

QString P190_creator::convertCoordinates(const QString &input)
{
    QRegExp rx("([0-9]+)° ([0-9]+)' ([0-9]+\\.[0-9]+)\" ([NSEW]), ([0-9]+)° ([0-9]+)' ([0-9]+\\.[0-9]+)\" ([NSEW])");
    QString result;

    if (rx.indexIn(input) != -1) {
        QString latDeg = rx.cap(1);
        QString latMin = rx.cap(2);
        QString latSec = rx.cap(3);
        QString latDir = rx.cap(4);
        QString lonDeg = rx.cap(5);
        QString lonMin = rx.cap(6);
        QString lonSec = rx.cap(7);
        QString lonDir = rx.cap(8);

        result = latDeg + latMin + latSec + latDir + lonDeg + lonMin + lonSec + lonDir;
    }

    return result;
}


QString P190_creator::replaceDates(QString& input) {
    // Define the current date in the desired format (dd.MM.yyyy)
    QDate date = QDate(2024, 1,1).addDays(this->dayOfSurvey-1);
    QString currentDate = date.toString("dd.MM.yyyy");
    QString modifiedInput = input;
    QString month = QString::number(date.month());
    QString day = QString::number(date.day());
    if (month.length() == 1) {
        month = "0" + month;
    }
    if (day.length() == 1) {
        day = "0" + day;
    }
    modifiedInput.replace("23", day);
    modifiedInput.replace("06", month);
    modifiedInput.replace("1997", QString::number(date.year()));

    return modifiedInput;
}

void P190_creator::fillSourceInfoInHeader(QStringList &header)
{
    calcItemsCount();
    for (auto i: MyVault->ItemsVault) {

    }
    for (int i = 0; i < header.size(); i++) {
        if (header[i].startsWith("H0103") || header[i].startsWith("\"H0103")) {

        }
    }
}

