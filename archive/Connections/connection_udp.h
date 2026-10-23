#ifndef CONNECTION_UDP_H
#define CONNECTION_UDP_H

#include "connection.h"
#include <QUdpSocket>
#include <QWidget>

class connection_UDP : public Connection
{
    Q_OBJECT
public:
    explicit connection_UDP();
    connection_UDP(QString IP, int port, QString filename);
    ~connection_UDP();


private:
    QString filename;
    QByteArray data;
    QString IP;
    int port;
    QUdpSocket* socket = nullptr;
    QTextEdit* nmeaWindow = nullptr;
    void reconnect() override;
private slots:
    void ReadyRead() override;
};

#endif // CONNECTION_UDP_H
