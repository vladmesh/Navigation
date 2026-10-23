#ifndef CONNECTIONLOGIC_H
#define CONNECTIONLOGIC_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <memory>
#include "connectionui.h"
#include "nmeaparser.h"

class Connection : public QObject
{
    Q_OBJECT

public:
    explicit Connection(QObject *parent = nullptr);
    virtual ~Connection();

    void recieve_data(QByteArray data);
    void create_file_for_nmea(QString filename);
    void write_nmea_data(QByteArray nmea_data);
    int check_nmea_data(QByteArray nmea_data);
    int check_sum_nmea(QByteArray nmea_data);
    int check_double_package(QByteArray nmea_data);
    float calcQuality(bool recieved);
    void setFilename(const QString &newFilename);
    virtual void reconnect() = 0;
    void stopTimer();
    QTimer *getTimer();
    std::unique_ptr<ConnectionUI> ui;
    QString lastRecievedGGA = "GGA empty";
    QString lastRecievedRMC = "RMC empty";
    QFile* file = nullptr;
    QString filename;
    QString itemName;

    void setItemName(const QString &newItemName);

private slots:
    virtual void ReadyRead();
    void onTimeout();
    void on_pushButton_clicked();

signals:
    void sendNewHasConnection(bool newNeedConnection);

private:
    bool showSignalQuality(float currentQuality);
    static NmeaParser nmeaParser;
    bool signalTimerAlert = false;
    QTimer* timer = nullptr;

    qsizetype calcWindow = 20; ///< размер окна для подсчёта качества, пакетов
    double border = 0.85;
    QVector<bool> packetsRecieved;
    float currentQuality = 0;
    qsizetype oldestPacket = 0; ///< индекс самого старого пакета в заполненном окне

    QTextStream* datastream = nullptr;

};

#endif // CONNECTIONLOGIC_H
