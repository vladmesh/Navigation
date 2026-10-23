#include "streamer.h"
#include "functions.h"

double Streamer::defaultChannelDepth = 0.5;

Streamer::Streamer(QObject *parent)
{}

Streamer::Streamer(QString name, FixedItem *towingPoint,
    double angleToWired, double absoluteLayback, uint NumChanels, QVector<double> chans)
    : TowedItem(name, towingPoint, angleToWired, absoluteLayback)
{
    this->NumChanels = NumChanels;
    this->chans = chans;
    itemType = "Streamer";

    if (NumChanels == 0 || chans.empty()) {
        qDebug() << "Wrong streamer configuration";
        return;
    }

    for (uint i = 1; i <= NumChanels; i++) {
        Channel* chan = new Channel(i);
        ChannelsVector.append(chan);
    }
    totalLength =  chans.at(chans.size() - 1);
}

Streamer::Streamer(QString name, FixedItem *towingPoint, double angleToWired,
    double absoluteLayback, uint NumChannels, QString chanStep)
    : TowedItem(name, towingPoint, angleToWired, absoluteLayback), NumChanels(NumChannels)
{
    bool stepIsStable = false;
    float dChan = 0;
    itemType = "Streamer";

    channelStep = chanStep;

    chanStep.toDouble(&stepIsStable);
    if (stepIsStable) {
        dChan = chanStep.toDouble();
        for (uint i = 0; i < NumChannels; i++) {
            chans.append(i*dChan);
        }
    } else { //1&2 48 chans
        if (chanStep == "1&2") {
            for (int i = 0; i < 24; i++) {
                chans.append(i);
                // qDebug() << chans[i];
            }
            for (int i = 0; i < 24; i++) {
                chans.append(chans[23] + 2 + i*2);
                // qDebug() << chans[24+i];
            }
        }
    }
    for (uint i = 1; i <= NumChanels; i++) {
        Channel* chan = new Channel(i);
        ChannelsVector.append(chan);
    }
    totalLength =  chans.at(chans.size() - 1);
}

Streamer::Streamer(QString name, FixedItem *towingPoint, double angleToWired, double absoluteLayback, QString singleChannelType)
    : TowedItem(name, towingPoint, angleToWired, absoluteLayback), singleChannelType(singleChannelType)
{
    NumChanels = 2;
    chans.append(0);
    chans.append(chans[0] + 0.8);
    if ("1x8" == singleChannelType) return;
    if ("1x16" == singleChannelType) {
        chans.append(chans[1] + 1.6);
        NumChanels++;
        return;
    } else {
        chans.append(chans[1] + 2.4);
        NumChanels++;
    }
    if ("1x24" == singleChannelType) return;
    chans.append(chans[2] + 4);
    NumChanels++;
    if ("1x48" == singleChannelType) return;
    chans.append(chans[3] + 7.2);
    NumChanels++;
    if ("1x48" == singleChannelType) return;
    if ("1x96" == singleChannelType) {
        return;
    } else {
        qWarning() << __FUNCTION__ << "Streamer type error";
    }
    totalLength = chans.at(chans.size() - 1);
}

void Streamer::calcChansCoors()
{
    // double azRad = azimuthOfMovement*M_PI/180;

    if (endBuoy != nullptr) {
        if (!endBuoy->hasConnection) {
            calcChansCoorsNoEndBuoy();
        } else {
            calcChansStreamerIsStrict();
        }
    } else {
        calcChansCoorsNoEndBuoy();
    }
    // qDebug() << realAzimuthOfTowingRadians << sharedCircularBuffer.calculateAzimuth();
    // ChannelsVector[0]->showAzDistToObj(ChannelsVector[ChannelsVector.count() - 1]);
    calcStreamerDepth();

}

void Streamer::calcChansCoorsNoEndBuoy()
{
    realAzimuthOfTowingRadians = -1;
    if (x_coor == 0 && y_coor == 0) return; //голова косы еще не рассчитана
    ChannelsVector[0]->x_coor = this->x_coor;
    ChannelsVector[0]->y_coor = this->y_coor;

    //направление для первичной расстановки каналов: продолжение линии буксировки,
    //если голова совпадает с точкой буксировки - обратный курс судна
    double backX = x_coor - towingPoint->x_coor;
    double backY = y_coor - towingPoint->y_coor;
    double backLen = qSqrt(backX*backX + backY*backY);
    if (backLen > 1e-6) {
        backX /= backLen;
        backY /= backLen;
    } else {
        double az = sharedCircularBuffer.calculateAzimuth() + M_PI;
        backX = qSin(az);
        backY = qCos(az);
    }

    //каждый канал догоняет предыдущий на расстоянии шага каналов (цепочка трактрис),
    //поэтому в поворотах коса идет по следу, а не разворачивается целиком
    for(int i = 1; i < ChannelsVector.size(); i++) {
        Channel* prev = ChannelsVector[i-1];
        Channel* cur = ChannelsVector[i];
        double spacing = chans[i] - chans[i-1];
        cur->height = this->height + chans[i]* dh;

        if (cur->x_coor == 0 && cur->y_coor == 0) {
            cur->x_coor = prev->x_coor + spacing*backX;
            cur->y_coor = prev->y_coor + spacing*backY;
            continue;
        }

        double dx = prev->x_coor - cur->x_coor;
        double dy = prev->y_coor - cur->y_coor;
        double dist = qSqrt(dx*dx + dy*dy);
        if (dist > spacing) {
            double scale = (dist - spacing) / dist;
            cur->x_coor += dx*scale;
            cur->y_coor += dy*scale;
        }
    }
}


void Streamer::calcChansStreamerIsStrict()
{
    realLen = pow(((x_coor - getChan(getChanCount()-1)->x_coor)*(x_coor - getChan(getChanCount()-1)->x_coor) +
                   (y_coor - getChan(getChanCount()-1)->y_coor)*(y_coor - getChan(getChanCount()-1)->y_coor)), 0.5); //real length of streamer

    distanceCalcCoef = totalLength >= realLen? 1.0: realLen/totalLength;
    ChannelsVector[0]->x_coor = this->x_coor;
    ChannelsVector[0]->y_coor = this->y_coor;

    // qDebug() << x_coor << y_coor << towingPoint->x_coor << towingPoint->y_coor << __FUNCTION__;

    QPointF pTowingPoint(towingPoint->x_coor, towingPoint->y_coor);
    QPointF pEndBuoy(endBuoy->x_coor, endBuoy->y_coor);
    double deltaX = pTowingPoint.x() - pEndBuoy.x();
    double deltaY = pTowingPoint.y() - pEndBuoy.y();
    realAzimuthOfTowingRadians = qAtan2(deltaX, deltaY);

    // qDebug() << pTowingPoint <<pEndBuoy;
    // qDebug() << __FUNCTION__ << qRadiansToDegrees(realAzimuthOfTowingRadians) << qRadiansToDegrees(sharedCircularBuffer.calculateAzimuth());
    // qDebug();

    for(int i = 1; i < ChannelsVector.size(); i++) {
        // ChannelsVector[i]->height = this->height + chans[i]* dh;
        ChannelsVector[i]->x_coor = x_coor - (1)*(chans[i]) *qSin(realAzimuthOfTowingRadians);
        ChannelsVector[i]->y_coor = y_coor - (1)*(chans[i]) *qCos(realAzimuthOfTowingRadians);
    }
    printChansCoor();
}


void Streamer::printChansCoor() //функция для дебага. выводит координаты ка
{
    int i = 0;
    // QDebug deb = qDebug();
    for (Channel* ch: ChannelsVector) {
        if (std::isnan(ch->x_coor)) {
            qDebug() <<qSetRealNumberPrecision(10)<< "Chan "
                     <<i<<"X "<<ch->x_coor<<"Y "<<ch->y_coor<<"Depth"<<ch->depth;
            i++;
        } else {
            return;
        }
    }
}

void Streamer::printPos()
{
    // logmsg(name + "\nHeight " + QString::number(height) +
    //        "\nUTM East:" + QString::number(x_coor) +
    //        "\nUTM North:" + QString::number(y_coor) +
    //        "\nGeo East:" + QString::number(this->longitude) +
    //        "\nGeo Notrh:" + QString::number(this->latitude) +
    //        "Azimuth of movement" + QString::number(azimuthOfMovement));
    printChansCoor();
}


Streamer::Channel* Streamer::getChan(uint number)
{
    if (number < 1){
        qDebug() << number<<" is out of channels range";
        return nullptr;
    } else if(number > ChannelsVector.size()) {
        return ChannelsVector.at(ChannelsVector.size() - 1);
    } else {
        return ChannelsVector.at(number-1);
    }
}

uint Streamer::getChanCount()
{
    return this->NumChanels;
}

QVector<double> Streamer::getChans() const
{
    return chans;
}

void Streamer::setTotalLength(double newTotalLength)
{
    totalLength = newTotalLength;
}

void Streamer::setEndBuoy(Buoy *newEndBuoy)
{

    endBuoy = newEndBuoy;
}

double Streamer::getTotalLength()
{
    return totalLength;
}

void Streamer::calcStreamerDepth()
{
    if (QString(towingPoint->metaObject()->className()) == "Buoy") {
        Buoy* leadBuoy = dynamic_cast<Buoy*>(towingPoint);
        double leadEndDepthDif = 0;
        if (this->endBuoy != nullptr) {
            leadEndDepthDif = leadBuoy->towingDepth - this->endBuoy->towingDepth;
        } else {
            leadEndDepthDif = 0;
        }
        for (int i = 0; i < NumChanels; i++) {
            ChannelsVector[i]->depth = leadBuoy->towingDepth - leadEndDepthDif*(chans[i]/totalLength);
        }
    } else {
        if (endBuoy != nullptr) {
            for (int i = 0; i < NumChanels; i++) {
                ChannelsVector[i]->depth = endBuoy->towingDepth;
            }
        } else {
            for (int i = 0; i < NumChanels; i++) {
                ChannelsVector[i]->depth = defaultChannelDepth;
            }
            // qDebug() << "Streamer hasn't depth information";
        }
    }
}

double Streamer::getDistanceCalcCoef() const
{
    return distanceCalcCoef;
}

Streamer::Channel::Channel(uint myNumber)
{
    this->myNumber = myNumber;
}

QString Streamer::Channel::getUTMPos()
{
    QString res(22, ' ');
    res.replace(4, 9,floatToQString(x_coor, 9,1));
    res.replace(13, 8,floatToQString(y_coor, 9,1));
    // qDebug() <<locale.toString(y_coor, 'f', 10)<<doubleToQString(y_coor, 10,2) << __FUNCTION__;

    res.replace(22, 4,floatToQString(depth, 4, 1));

    for (int i = 0; i < 4; i++) {
        res[i] = floatToQString(myNumber, 4, -1)[i];
    }
    // qDebug() << res;
    return res;
}


