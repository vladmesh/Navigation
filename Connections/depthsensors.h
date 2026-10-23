#ifndef DEPTHSENSORS_H
#define DEPTHSENSORS_H

#include <QVector>
#include <QUdpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>

// #include "logger.h"

class DepthSensors
{
public:
    DepthSensors();
    ~DepthSensors();

    void createCalculationScheme();
    void calcDepth();
    void fillSensorsDepth();
    void setChans(const QVector<float> &newChans);

    QVector<float> getChanDepth() const;

private:
    struct nearestSensors{
        int prev;
        int next;
        float prevCoef;
        float nextCoef;
    };

    int checkNS();

    int numOfSensors = 0;
    QVector<float> sensorsPos;
    QVector<float> sensorsDepth;
    QVector<float> chans;
    QVector<nearestSensors> nS;

    QVector<float> chanDepth;

    bool isEndBuoy;
    bool isLeadBuoy;
    float totalLength;

    float leadBuoyDepth;
    float endBuoyDepth;

    QTimer* timer = nullptr;
    // Logger* log = nullptr;


private slots:
    void ReadyRead();
};

#endif // DEPTHSENSORS_H
