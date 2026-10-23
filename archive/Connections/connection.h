#ifndef CONNECTION_H
#define CONNECTION_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QSerialPort>
#include <QMessageBox>
#include <QException>
#include <QList>
#include <QDir>
#include <QEvent>
#include <QCloseEvent>

#include "nmeaparser.h"
// #include "logger.h"
// #include "qevent.h"

namespace Ui {
class Connection;
}
/**
 * @class Connection
 * @brief виртуальный для обработки соединения
 * в нем реализованы прототипы функций, методы для обработки и записи данных
 * и отправка сигналов объекту-владельцу соединения
 */
class Connection : public QWidget
{
    Q_OBJECT

public:
    explicit Connection(QWidget *parent = nullptr);
    virtual ~Connection();

    void recieve_data(QByteArray data); //method for data acquisistion
    void create_file_for_nmea(QString filename); //creating file for NMEA saving



    void write_nmea_data(QByteArray nmea_data);

    ///Quality of data trasmition
    int check_nmea_data(QByteArray nmea_data);
    int check_sum_nmea(QByteArray nmea_data);

    int check_double_package(QByteArray nmea_data);

    //счетчик качества
    uint calcWindow = 20; // размер окна для подсчета качества
    float border = (float)0.85;
    QVector<bool> packetsRecieved;


    float calcQuality(bool recieved);
    //счетчик качества
    float currentQuality = 0;
    QVector<bool>::iterator iter;
    ///Quality of data trasmition

    QDir logsDir = QDir("~/Documents/Ship_logs");
    QString filename;
    QFile* file = nullptr;
    QTextStream* datastream = nullptr;

    QString lastRecievedGGA = "GGA empty";
    QString lastRecievedRMC = "RMC empty";

    void setFilename(const QString &newFilename);
    virtual void reconnect() = 0;


    void stopTimer();
    QTimer *getTimer();

private:
    bool showSignalQuality(float currentQuality);
    Ui::Connection *ui;
    static NmeaParser nmeaParser;
    bool signalTimerAlert = false;
    QTimer* timer = nullptr;
    // Logger* log = nullptr;


    void closeEvent(QCloseEvent* event) override{
        hide();
        event->ignore();
    }
    // FixedItem* myItem;

private slots:
    virtual void ReadyRead(); //slot for getting signal
    void onTimeout();
    // void readPendingDatagrams(); //to TCP

    void on_pushButton_clicked();

signals:
    void sendNewHasConnection(bool newNeedConnection);
};

#endif // CONNECTION_H
