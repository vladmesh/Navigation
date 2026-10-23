#include "connection_udp.h"

connection_UDP::connection_UDP() {}

connection_UDP::connection_UDP(QString IP, int port, QString filename)
    : filename(filename), IP(IP), port(port)
{
    create_file_for_nmea(this->filename);
    this->lastRecievedGGA = "empty net";
    socket = new QUdpSocket();
    connect(socket, &QUdpSocket::readyRead, this, &connection_UDP::ReadyRead);
    socket->connectToHost(QHostAddress(IP), port);

    setWindowTitle(IP + " "+ QString::number(port));
}

connection_UDP::~connection_UDP() {
    file->close();
    socket->close();
    delete socket;
}

void connection_UDP::reconnect()
{

}


void connection_UDP::ReadyRead()
{
    QByteArray data = socket->readAll();
    QByteArray packageType= data.mid(3, 3);
    if ((packageType== "VTG" || packageType== "ZDA" || packageType== "GBS" ||
         packageType== "DTM" || packageType== "VLW" || packageType== "GNS" ||
         packageType== "GSV" || packageType== "GLL" || packageType== "GRS" ||
         packageType== "GSA" || packageType== "GST" || packageType== "VLW")) {
        qDebug() << packageType << " filtred" << data <<__FUNCTION__;
        data.clear();
        return; //ignoring packages if not GGA, RMC or HDT for not acepting them as bad packages
    }

    if (check_nmea_data(data)) {
        recieve_data(data);
        calcQuality(true);
        qDebug() << "GOOD DATA "<< data;
    } else {
        calcQuality(false);
        qDebug() << "BAD DATA "<< data;
    }
}
