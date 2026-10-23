#include "sender.h"

Sender::Sender() : socket(new QTcpSocket(this)), needSendPackage(false){
    sendingTimer = new QTimer();
    sendingTimer->start(1000);
    connect(sendingTimer, &QTimer::timeout, this, &Sender::sendPackage);
}

Sender::~Sender()
{
    if (socket->isOpen()) {
        socket->close();
    }
}


QStringList Sender::createPackage()
{
    QStringList packageList;
    QJsonArray itemsArray;
    if (MyVault == nullptr) return QStringList();
    for (FixedItem* item : MyVault->ItemsVault) {
        if (!item) continue;
        QJsonObject itemObject;
        itemObject["name"] = item->name;
        itemObject["itemType"] = item->itemType;
        itemObject["x_coor"] = item->x_coor;
        itemObject["y_coor"] = item->y_coor;
        if (item->itemType == "Streamer") {
            Streamer* strm = qobject_cast<Streamer*>(item);
            if (strm) {
                itemObject["Chan count"] = (int)strm->getChanCount();
                QJsonArray channelsArray;
                for (int i = 0; i < (int)strm->getChanCount(); i++) {
                    QJsonObject channelObject;
                    channelObject["x"] = strm->getChan(i+1)->x_coor;
                    channelObject["y"] = strm->getChan(i+1)->y_coor;
                    channelObject["depth"] = strm->getChan(i+1)->depth;
                    channelsArray.append(channelObject);
                }
                itemObject["Channels"] = channelsArray;
            }
        }

        itemsArray.append(itemObject);
    }

    QJsonObject rootObject;
    rootObject["objects"] = itemsArray;

    QJsonDocument doc(rootObject);
    packageList.append(doc.toJson(QJsonDocument::Indented));
    return packageList;
}

void Sender::sendPackage()
{
    if (!needSendPackage) return;
    QStringList package = createPackage();
    socket->write(QString::number(package.size()).toStdString().c_str());
    for (QString i: package) {
        socket->write(i.toStdString().c_str());
    }
    socket->write("\n\n");
}

void Sender::setIP(const QHostAddress &newIP)
{
    IP = newIP;
}

void Sender::setPort(int newPort)
{
    port = newPort;
}

void Sender::setNeedSendPackage(bool newNeedSendPackage)
{
    needSendPackage = newNeedSendPackage;
}

void Sender::setMyVault(ItemsStorage *newMyVault)
{
    MyVault = newMyVault;
}

int Sender::getPort() const
{
    return port;
}

QHostAddress Sender::getIP() const
{
    return IP;
}

bool Sender::connectToHost()
{
    socket->disconnectFromHost();
    delete socket;
    socket = new QTcpSocket();
    qDebug() << IP << port <<__FUNCTION__;
    socket->connectToHost(IP.toString(), port);

    return socket->waitForConnected();
}


void Sender::disconnectFromHost() {
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->disconnectFromHost();
        socket->waitForDisconnected();
    }
}
