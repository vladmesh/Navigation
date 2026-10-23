#include "marimag.h"

MariMag::MariMag() {}

MariMag::MariMag(QString name, FixedItem *towingPoint, double angleToWired, double absoluteLayback)
: TowedItem(name, towingPoint, angleToWired, absoluteLayback)
{
    itemType = "MariMag";
}

void MariMag::initDummyTowingPoint(double layBack)
{
    hasDummyTowingPoint = true;
    Q_UNUSED(layBack)
    // towingPoint = new FixedItem(0,0,0,"Dummy");
}

void MariMag::calcIFNotConnected()
{
    if (currentFullMagData == nullptr || !hasDummyTowingPoint) { //если у нас нет информации о координатах из текстового файла
        TowedItem::calcIFNotConnected();
    } else {
        setWireLength(currentFullMagData->commonData.layback);
        QPointF utmCoor = nmeaParser.GeoToUTM(QGeoCoordinate(currentFullMagData->commonData.lat, currentFullMagData->commonData.lon));
        towingPoint->x_coor = utmCoor.x();
        towingPoint->y_coor = utmCoor.y();
        TowedItem::calcIFNotConnected();
        if (hasSecondMagnitometer) {
            secondMagnitometer->setWireLength(currentFullMagData->commonData.layback + currentFullMagData->commonData.baseline);
            secondMagnitometer->TowedItem::calcIFNotConnected();

            // тут поправь братик
        }
    }
}

// void MariMag::initDummyTowingPoint(double layBack)
// {
//     if (towingPoint == nullptr) {
//         towingPoint = new FixedItem(0,0,0,"Dummy");
//         this->wireLength = layBack;
//         qDebug() << __FUNCTION__;
//     }
// }

