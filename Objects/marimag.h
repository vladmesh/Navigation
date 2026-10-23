#ifndef MARIMAG_H
#define MARIMAG_H

#include "toweditem.h"
#include "marimagparser.h"

class MariMag : public TowedItem
{

    friend class Fabric;
public:
    MariMag();
    MariMag(QString name, FixedItem* towingPoint,
            double angleToWired, double absoluteLayback);

    ///@brief function for testing or calculating
    /// pos for MariMag without creating real towing point
    void initDummyTowingPoint(double layBack);


    void calcIFNotConnected() override;
    bool hasDummyTowingPoint = false;

    double field;
    double squareError;
    double alt;
    double pitch;
    double roll;
    //height defined in FixedItem

    bool hasSecondMagnitometer = false;
    QPointer<MariMag> secondMagnitometer;

    MariMagParser::FullData* currentFullMagData = nullptr;
};

#endif // MARIMAG_H
