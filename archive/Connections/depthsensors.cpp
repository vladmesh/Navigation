#include "depthsensors.h"

DepthSensors::DepthSensors() {

}

DepthSensors::~DepthSensors()
{

}

//заполнение структурок для каждого канала,
//по которым будет происходить вычесление в
//calcDepth()
void DepthSensors::createCalculationScheme()
{
    if (isLeadBuoy) {
        sensorsDepth.prepend(leadBuoyDepth);
        sensorsPos.prepend(0);
    }
    if (isEndBuoy) {
        sensorsDepth.append(leadBuoyDepth);
        sensorsPos.append(0);
    }
    for (int i = 0; i < chans.size(); i++) {
        nearestSensors a;
        if (chans[i] <= sensorsPos[0]) {
            a.prev = -1;
            a.prevCoef = 0;
            a.next = 0;
            a.nextCoef = 1;
        } else if (chans[i] >= sensorsPos[sensorsPos.size() - 1]) {
            a.prev = sensorsPos.size() - 1;
            a.prevCoef = 1;
            a.next = -1;
            a.nextCoef = 0;
        } else {
            for (int j = 0; j < sensorsPos.size(); i++) {
                if (chans[i] > sensorsPos[j]) {
                    a.prev = j;
                    a.prevCoef = 1 - (chans[i] - sensorsPos[j])/(sensorsPos[j+1] - sensorsPos[j]);
                    a.next = j+1;
                    a.nextCoef = 1 - (sensorsPos[j+1] - chans[i])/(sensorsPos[j+1] - sensorsPos[j]);
                }
            }
        }
        nS.append(a);
    }
}

//расчет глубин по схеме
void DepthSensors::calcDepth()
{
    chanDepth.clear();
    for (int i = 0; i < chans.size(); i++) {
        nearestSensors ns = nS[i];
        if (ns.prev == -1) {
            chanDepth.append(sensorsDepth[ns.next]);
        } else if (ns.next == -1) {
            chanDepth.append(sensorsDepth[ns.prev]);
        } else {
            chanDepth.append(sensorsDepth[ns.prev] * ns.prevCoef + sensorsDepth[ns.next] * ns.nextCoef);
        }
    }
}


//
void DepthSensors::fillSensorsDepth()
{
    for (int i = 0; i < numOfSensors; i++) {
        sensorsDepth[i] = 0.2;
    }
}

void DepthSensors::setChans(const QVector<float> &newChans)
{
    chans = newChans;
}

QVector<float> DepthSensors::getChanDepth() const
{
    return chanDepth;
}

int DepthSensors::checkNS()
{
    for (auto i: nS) {
        if ((i.nextCoef + i.prevCoef - 1) > 0.05 || (i.nextCoef + i.prevCoef - 1) < -0.05) {
            return 0;
        }
        if (i.next > 0 && sensorsDepth[i.next] < 0) return 0;
        if (i.prev > 0 && sensorsDepth[i.prev] < 0) return 0;
    }
    return 1;
}
