#include "fixeditem.h"

double FixedItem::realAzimuthOfTowingRadians = -1;
CircularBuffer FixedItem::sharedCircularBuffer(10);

FixedItem::FixedItem() {}

FixedItem::FixedItem(double x,double y,double z, QString name) :
    x(x), y(y), z(z), name(name)
{
    itemType = "Fixed";
    qDebug() <<itemType<< " Item Created "<<x<<y<<name;
    CircularBuffer& buffer = FixedItem::sharedCircularBuffer;

    this->creationPriority = 0;
}

FixedItem::~FixedItem() {
    qDebug() <<itemType<< " Item Deleted " <<x<<y<<name << "~FixedItem";
    if (connection != nullptr) {
        delete connection;
    }
    deletingWires();
}

void FixedItem::calcItemCoordinates()
{
    if (hasConnection && connection == nullptr) {
        qDebug() << "needCalculate = false";
        needCalculate = false;
    }
    if (!needCalculate && hasConnection && connection != nullptr) {
        qDebug() << "needCalculate = true";
        needCalculate = true;
    }

    if (hasConnection) {
        calcIfConnected();
    } else {
        if (itemType == "Fixed" && ItemForCalculations == nullptr) {
            qWarning() << __FUNCTION__ << name<< "Error: no ItemForCalculations and no connection";
            return;

        } else {
            calcIFNotConnected();
        }
    }
}

void FixedItem::calcIfConnected()
{
    x_coor = lastGGAData.coorUTM.rx();
    y_coor = lastGGAData.coorUTM.ry();
    latitude = lastGGAData.coordinate.latitude();
    longitude = lastGGAData.coordinate.longitude();
    height = lastGGAData.height;

    if ((amIItemForCalculating && itemType == "Fixed") || ItemForCalculations == this) {
        sharedCircularBuffer.add(lastGGAData.coordinate);
    }

    azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();
}

void FixedItem::calcIFNotConnected()
{
    if (QString(metaObject()->className()) == "FixedItem") {
        azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();
        if (azimuthOfMovement == -1) {
            azimuthOfMovement = this->realAzimuthOfTowingRadians;
        }
    } else {
        azimuthOfMovement = ItemForCalculations->azimuthOfMovement;
    }
    double azRad = sharedCircularBuffer.calculateAzimuth();
    if (azRad == -1) {
        azRad = realAzimuthOfTowingRadians - M_PI;
    }
    if (azRad > 2*M_PI) azRad -= 2*M_PI;
    x_coor = ItemForCalculations->x_coor + (y-ItemForCalculations->y)*qCos(azRad) +
             (x-ItemForCalculations->x)*qSin(azRad);
    y_coor = ItemForCalculations->y_coor - (y-ItemForCalculations->y)*qSin(azRad) +
             (x-ItemForCalculations->x)*qCos(azRad);


    QGeoCoordinate curGeoCoordinate = nmeaParser.UTMtoGeo(QPointF(x_coor, y_coor));

    latitude = curGeoCoordinate.latitude();
    longitude = curGeoCoordinate.longitude();
    height = ItemForCalculations->height - (z - ItemForCalculations->z);
}


void FixedItem::deletingWires() {
    if (vectorOfConnected.isEmpty()) return;
    for (auto item: vectorOfConnected) {
        if (item) {
            item->lostWire();
        }
    }
}

void FixedItem::lostWire(){
    qDebug() << "Wire Lost Slot activated " << this->name;
    delete this;
}

QPointF FixedItem::getCoordinates()
{
    return QPointF(x_coor, y_coor);
}

QString FixedItem::getLastGGA()
{
    QString newGGA = "empty GGA";
    if (connection != nullptr && hasConnection) {
        newGGA = connection->lastRecievedGGA;
        lastGGAData = nmeaParser.parseNmeaGGA(newGGA);
    }
    return newGGA;
}

QString FixedItem::getLastRMC()
{
    QString newRMC = "empty RMC";
    if (connection != nullptr && hasConnection) {
        newRMC = connection->lastRecievedRMC;
        lastRMCData = nmeaParser.parseNmeaRMC(newRMC);
    }
    return newRMC;
}



void FixedItem::setItemForCalculations(FixedItem *newItemForCalculations)
{
    if (newItemForCalculations->hasConnection == false) return;
    ItemForCalculations = newItemForCalculations;
    newItemForCalculations->amIItemForCalculating = true;

}

bool FixedItem::showAzDistToObj(FixedItem *to)
{
    QPointF pOther(to->x_coor, to->y_coor);
    QPointF pMy(x_coor, y_coor);
    double dist = pow(((pMy.x() - pOther.x()) * (pMy.x() - pOther.x()) + ((pMy.y() - pOther.y()) * (pMy.y() - pOther.y()))),0.5);
    double deltaX = pMy.x() - pOther.x();
    double deltaY = pMy.y() - pOther.y();
    double azimuthDegrees = (M_PI + qAtan2(deltaX, deltaY)) * 180 / M_PI;
    if (azimuthDegrees < 0) {
        azimuthDegrees += 360.0;
    }
    qDebug() << "Between " << name << " and " << to->name << dist << "Azimuth = " <<azimuthDegrees;
    return true;
}

void FixedItem::hasConnectionArrived(bool newHasConnection)
{
    static bool firstPackageArrived = false; //не бейте лучше обоссыте
    if (hasConnection != newHasConnection || !firstPackageArrived)   {
        firstPackageArrived = true;
        qDebug() << "old" << hasConnection <<"new" << newHasConnection << __FUNCTION__;

        if (amIItemForCalculating) {
            qDebug();
            qWarning() << name<< " не имеет соединения" << __FUNCTION__;
            hasConnection = newHasConnection;
        } else {
            hasConnection = newHasConnection;
        }
        emit hasConnectionChanged(this);
    }
}

