#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QMessageBox>
#include <QException>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>


#include "itemsstorage.h"
// #include "logger.h"

namespace Ui {
class Sender;
}

class Sender : public QObject
{
    Q_OBJECT
public:
    Sender();
    ~Sender();
    // Logger* log;

    QStringList createPackage();
    bool connectToHost();

    void disconnectFromHost();

    QHostAddress getIP() const;
    void setIP(const QHostAddress &newIP);

    int getPort() const;
    void setPort(int newPort);

    void setNeedSendPackage(bool newNeedSendPackage);

    void setMyVault(ItemsStorage* newMyVault);



public slots:
    void sendPackage();
private:
    ItemsStorage* MyVault = nullptr;
    QTimer* sendingTimer;
    QHostAddress IP = QHostAddress::LocalHost;
    int port = 5555;
    QTcpSocket* socket = nullptr;
    bool needSendPackage = false;
};
#endif // SENDER_H
