#include "connectionlogic.h"
#include "qexception.h"
#include "ui_connectionui.h"

Connection::Connection(QObject *parent)
    : QObject(parent), ui(std::make_unique<ConnectionUI>())
{
    ui->hide();
    packetsRecieved.reserve(calcWindow);
    iter = packetsRecieved.end();
    timer = new QTimer(this);
    timer->setInterval(5000);
    connect(timer, &QTimer::timeout, this, &Connection::onTimeout);
    connect(ui->getPushButton(), &QPushButton::clicked, this, &Connection::on_pushButton_clicked);
}

Connection::~Connection()
{
    file->close();
    delete timer;
}

void Connection::recieve_data(QByteArray data)
{
    qDebug() << __FUNCTION__ << data;
    if (check_double_package(data)){
        QStringList nmeaMessages = QString(data).split("\r\n", Qt::SkipEmptyParts);
        write_nmea_data(nmeaMessages[0].append("\r\n").toUtf8());
        write_nmea_data(nmeaMessages[1].append("\r\n").toUtf8());
        return;
    }
    write_nmea_data(data);
    ui->setLabelText(data);
}

void Connection::create_file_for_nmea(QString filename)
{
    if (!file) {
        file = new QFile(filename);
    } else {
        if (file->isOpen()) file->close();
    }

    file->open(QIODevice::Append);
    if (!file->isOpen()) {
        QMessageBox::critical(nullptr, "Error", "File not created");
        return;
    }

    datastream = new QTextStream(file);
    this->filename = filename;
    timer->start();
}

void Connection::write_nmea_data(QByteArray nmea_data)
{
    int data_condition;
    timer->start();
    signalTimerAlert = false;
    if (file && datastream) {
        data_condition = check_nmea_data(nmea_data);
        if (data_condition == 1) {
            *datastream << nmea_data;
            lastRecievedGGA = QString(nmea_data);
        } else if (data_condition == 3) {
            lastRecievedRMC = QString(nmea_data);
            *datastream << nmea_data;
        } else if (data_condition == 2) {
            *datastream << nmea_data;
            lastRecievedRMC = QString(nmea_data);
        } else {
            qDebug() << "BAD PACKAGE" << nmea_data;
            return;
        }
    } else {
        qDebug() << "File/Datastream not created";
    }
}

int Connection::check_nmea_data(QByteArray nmea_data)
{
    qDebug() << nmea_data;
    QByteArray hdt = nmea_data.mid(3, 3);
    if (!(hdt == "HDT" || hdt == "GGA" || hdt == "RMC")) return 0;

    if (nmea_data.length() < 65) {

        if (hdt == "HDT") {
            return 2;
        }

        qDebug() << "too short check_nmea_data" << nmea_data;
        return 0;
    }
    if (nmea_data[0] != '$') {
        qDebug() << "NOT $ check_nmea_data"<< nmea_data;
        return 0;
    }
    if (nmea_data.indexOf("\r\n") == -1) {
        qDebug() << "NOT \r\n check_nmea_data"<< nmea_data;
        return 0;
    }
    if (nmea_data.length() > 89) {
        qDebug() << "too long string" << nmea_data;
        return 2;
    }
    QByteArray rmc = nmea_data.mid(3, 3);
    if (rmc == "RMC") {
        return 3;
    }
    return 1;
}

int Connection::check_sum_nmea(QByteArray nmea_data)
{
    uint8_t calcChecksum = 0;
    int packetEndIndex = nmea_data.indexOf('*');
    for (int i = 1; i < packetEndIndex; ++i) {
        calcChecksum = calcChecksum ^ nmea_data[i];
    }
    uint8_t nibble1 = (calcChecksum & 0xF0) >> 4;
    uint8_t nibble2 = calcChecksum & 0x0F;
    uint8_t translatedByte1 = (nibble1 <= 0x9) ? (nibble1 + '0') : (nibble1 - 10 + 'A');
    uint8_t translatedByte2 = (nibble2 <= 0x9) ? (nibble2 + '0') : (nibble2 - 10 + 'A');
    uint8_t receivedCS0 = nmea_data[nmea_data.length() - 4];
    uint8_t receivedCS1 = nmea_data[nmea_data.length() - 3];
    if (translatedByte1 == receivedCS0 && translatedByte2 == receivedCS1) {
        return 1;
    } else {
        qDebug() << QChar(translatedByte1) << QChar(translatedByte2) << "VS" << QChar(receivedCS0) << QChar(receivedCS1) << __FUNCTION__;
        return 0;
    }
}

int Connection::check_double_package(QByteArray nmea_data)
{
    try {
        QStringList nmeaMessages = QString(nmea_data).split("\r\n", Qt::SkipEmptyParts);
        if (nmeaMessages.size() == 2) {
            return 1;
        }
    } catch (QException &ex) {
        qDebug() << ex.what();
        return 0;
    }
    return 0;
}

float Connection::calcQuality(bool recieved)
{
    if (packetsRecieved.size() < calcWindow) {
        packetsRecieved.push_front(recieved);
        if (recieved) {
            currentQuality += (1.0 / calcWindow);
        }
        if (packetsRecieved.size() == 20) iter = packetsRecieved.begin();
        return currentQuality * packetsRecieved.size() / calcWindow;
    }

    if (*iter) {
        currentQuality -= (1.0 / calcWindow);
    }
    if (recieved) {
        currentQuality += (1.0 / calcWindow);
    }

    *iter = recieved;
    if (iter == packetsRecieved.end()) {
        iter = packetsRecieved.begin();
    } else {
        iter++;
    }

    showSignalQuality(currentQuality);
    return currentQuality;
}

void Connection::setFilename(const QString &newFilename)
{
    filename = newFilename;
}


void Connection::stopTimer()
{
    timer->stop();
}

QTimer *Connection::getTimer()
{
    return timer;
}

void Connection::setItemName(const QString &newItemName)
{
    itemName = newItemName;
    if (ui) {
        ui->setWindowTitle(ui->windowTitle() + "    " + newItemName);
    }
}

void Connection::ReadyRead()
{
    // Реализация чтения данных
}

void Connection::onTimeout()
{
    bool newHasConnection;
    signalTimerAlert = true;
    newHasConnection = showSignalQuality(currentQuality);
    reconnect();
    emit sendNewHasConnection(newHasConnection);
}

void Connection::on_pushButton_clicked()
{
    delete this;
}

bool Connection::showSignalQuality(float currentQuality)
{
    bool res = true;
    if (currentQuality > border && !signalTimerAlert) {
        ui->setSignalQualityLabel("GOOD", Qt::green);
        res = true;
    } else {
        ui->setSignalQualityLabel("BAD", Qt::red);
        res = false;
    }
    emit sendNewHasConnection(res);
    return res;
}
