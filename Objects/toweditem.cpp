#include "toweditem.h"
#include "buoy.h"
#include "streamer.h"


TowedItem::TowedItem() {}

TowedItem::TowedItem(QString Name,
                     FixedItem* towingPoint, double angle = 0, double absoluteLayback = 5)
    : towingPoint(towingPoint), angle(angle), absoluteLayback(absoluteLayback)
{
    this->name = Name;


    if (QString(towingPoint->metaObject()->className()) != "FixedItem"
        && this->absoluteLayback < dynamic_cast<TowedItem*>(towingPoint)->absoluteLayback) {
        qDebug() << "absolute Layback Increased" << __FUNCTION__;
        this->absoluteLayback += dynamic_cast<TowedItem*>(towingPoint)->absoluteLayback;
    }
    if (QString(towingPoint->metaObject()->className()) == "FixedItem") {
        wireLength = absoluteLayback;
    } else {
        wireLength = absoluteLayback - dynamic_cast<TowedItem*>(towingPoint)->absoluteLayback;
    }

    // if (QString(towingPoint->metaObject()->className()) == "Streamer") {
    //     wireLength -= dynamic_cast<Streamer*>(towingPoint)->getTotalLength();
    // }


    itemType = "Towed";

    x = towingPoint->x - wireLength;
    y = towingPoint->y;
    z = 0;
    qDebug() <<QString(this->metaObject()->className())<< " Item Created "<<x<<y<<z<<name <<" towed by "<<towingPoint->name;

    // qDebug() << name <<
    towingPoint->vectorOfConnected.append(this);
    qDebug() << "TowedItem::TowedItem" << towingPoint->name << name;

    // if (towingPoint->hasConnection) {
    //     azimuthOfMovement = towingPoint->azimuthOfMovement;
    // } else {
        azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();
    // }
}

TowedItem::~TowedItem() {
    qDebug() << itemType << " Item Deleted " <<x<<y<<z<<name << "~TowedItem";
    if (hasConnection && connection != nullptr) delete this->connection;
}

void TowedItem::updateLayout()
{
    x = towingPoint->x * 1 - wireLength;
    y = towingPoint->y;
    z = towingPoint->z;
}

void TowedItem::calcIFNotConnected()
{
    NmeaParser parser;
    // double azRad = -1;
    if (realAzimuthOfTowingRadians == -1) { // если нам неизвестен реальный азимут буксировки
        // драггинг алгоритм, основан наалгоритме погони https://habr.com/ru/articles/254485/


        double dx;
        double dy;
        if (towingPoint->itemType == "Streamer") {
            Streamer* strm = dynamic_cast<Streamer*>(towingPoint);
            //расчитываем от последнего канала косы
            dx = strm->getChan(strm->getChanCount())->x_coor - x_coor;
            dy = strm->getChan(strm->getChanCount())->y_coor - y_coor;
        } else {
            dx = towingPoint->x_coor - x_coor;
            dy = towingPoint->y_coor - y_coor;
        }


        double distance = std::sqrt(dx * dx + dy * dy);
        // Если расстояние больше длины троса, перемещаем объект
        double heightDiff = towingPoint->z - z;
        double trueWireLength = std::sqrt(wireLength*wireLength - heightDiff*heightDiff);
        if (towingPoint->itemType == "Streamer") {
            Streamer* strm = dynamic_cast<Streamer*>(towingPoint);
            trueWireLength -= strm->totalLength;
        }


        if (distance > trueWireLength) {
            double scale = (distance - trueWireLength) / distance; // Масштабируем вектор
            x_coor = x_coor + dx * scale;
            y_coor = y_coor + dy * scale;
        }
        azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();
    } else {
        double dx = towingPoint->x_coor - x_coor;
        double dy = towingPoint->y_coor - y_coor;
        double distance = std::sqrt(dx * dx + dy * dy);
        double heightDiff = towingPoint->z - z;
        double heightDiffCoef = qCos(qAsin(heightDiff / distance));

        // Если расстояние больше длины троса, перемещаем объект
        // double trueWireLength = std::sqrt(wireLength*wireLength - heightDiff*heightDiff);

        // if (distance > trueWireLength) {
        //     double scale = (distance - trueWireLength) / distance; // Масштабируем вектор
        //     x_coor = x_coor + dx * scale;
        //     y_coor = y_coor + dy * scale;
        // }
        // azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();
        x_coor = towingPoint->x_coor + (x-towingPoint->x)*qSin(realAzimuthOfTowingRadians) * heightDiffCoef + (y-towingPoint->y)*qCos(realAzimuthOfTowingRadians) * heightDiffCoef;
        y_coor = towingPoint->y_coor + (x-towingPoint->x)*qCos(realAzimuthOfTowingRadians) * heightDiffCoef - (y-towingPoint->y)*qSin(realAzimuthOfTowingRadians) * heightDiffCoef;
    }

    QGeoCoordinate tmp = parser.UTMtoGeo(QPointF(x_coor, y_coor));
    this->latitude = tmp.latitude();
    this->longitude = tmp.longitude();
    if (QString(metaObject()->className()) == "Streamer") {
        dynamic_cast<Streamer*>(this)->calcChansCoors();
    }
}


void TowedItem::setWireLength(double newWireLength)
{
    wireLength = newWireLength;
}

QString TowedItem::getLastGGA()
{
    QString newGGA = "empty GGA";
    if (connection != nullptr && hasConnection) {
        newGGA = connection->lastRecievedGGA;
        lastGGAData = nmeaParser.parseNmeaGGA(newGGA);
        // connection->lastRecievedGGA = "empty GGA";
        // parser.printNmeaGGAData(lastGGAData);
    } else if(towingPoint != nullptr) {
        newGGA = towingPoint->getLastGGA();
        // lastGGAData = towingPoint->lastGGAData;
        // connection->lastRecievedRMC = "empty RMC";
        lastGGAData = nmeaParser.parseNmeaGGA(newGGA);
    }
    return newGGA;
}

QString TowedItem::getLastRMC()
{
    QString newRMC = "empty RMC";
    if (connection != nullptr && hasConnection) {
        newRMC = connection->lastRecievedRMC;
        lastRMCData = nmeaParser.parseNmeaRMC(newRMC);
        // parser.printNmeaRMCData(lastRMCData);
    } else if(towingPoint != nullptr) {
        newRMC = towingPoint->getLastRMC();
        lastRMCData = towingPoint->lastRMCData;
    }
    return newRMC;
}

void TowedItem::setBoardHeight(double newBoardHeight)
{
    z += boardHeight;
    z -= newBoardHeight;
    boardHeight = newBoardHeight;
}


