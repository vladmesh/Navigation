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
#define CHAINALGO
#ifdef CHAINALGO
            calcChansStreamerIsStrict();
#else
            calcChansByChainAlgorithm();
#endif
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

void Streamer::calcChansByChainAlgorithm() { //пока не используется
    if (!endBuoy) {
        return;
    }
    if (!endBuoy->hasConnection) {
        return;
    }
    realLen = qSqrt(qPow(endBuoy->x_coor - x_coor, 2) + qPow(endBuoy->y_coor - y_coor, 2));
    if ((this->endBuoy->absoluteLayback - this->absoluteLayback) < realLen) {
        qWarning() << "totalLength < realLen!";
        // calcChansCoors();
        calcChansStreamerIsStrict();
        return;
    }
    ChannelsVector[0]->x_coor = this->x_coor;
    ChannelsVector[0]->y_coor = this->y_coor;

    double azimuthOfMovement = sharedCircularBuffer.calculateAzimuth();

    double a = findA();
    if (a == -1) {
        calcChansStreamerIsStrict();
        return;
    }
    // Расчет угла между началом и концом косы
    QPointF pTowingPoint(towingPoint->x_coor, towingPoint->y_coor);
    QPointF pEndBuoy(endBuoy->x_coor, endBuoy->y_coor);
    double deltaX = pTowingPoint.x() - pEndBuoy.x();
    double deltaY = pTowingPoint.y() - pEndBuoy.y();
    realAzimuthOfTowingRadians = qAtan2(deltaX, deltaY);
    if (realAzimuthOfTowingRadians < 0) realAzimuthOfTowingRadians + M_PI/2;

    qDebug() << realAzimuthOfTowingRadians*(180/M_PI) << azimuthOfMovement*(180/M_PI) << __FUNCTION__;
    endBuoy->showAzDistToObj(this);

    // double scale = this->realLen / this->totalLength;
    QVector<double> x_graph(this->NumChanels * 300);
    QVector<double> y_graph(this->NumChanels * 300);
    QVector<double> x_local(this->NumChanels);
    QVector<double> y_local(this->NumChanels);
    // Вычисляем x_graph и y_graph

    x_local[0] = 0;
    y_local[0] = 0;
    int curChannel = 1;
    double dist;

    for (int i = 1; i < this->NumChanels * 300; ++i) {
        x_graph[i] = this->totalLength*(double)i/(this->NumChanels * 300);
        y_graph[i] = a * cosh((x_graph[i] - realLen / 2.0) / a) - a * cosh(-realLen / (2.0 * a));
        dist = pow(pow(x_graph[i]-x_local[curChannel-1], 2) + pow(y_graph[i]-y_local[curChannel-1], 2), 0.5);
        if (dist >= (chans[curChannel]-chans[curChannel-1])) {
            x_local[curChannel] = x_graph[i];
            y_local[curChannel] = y_graph[i];
            curChannel++;
            if (curChannel == this->NumChanels) break;
        }
    }



    if ((azimuthOfMovement * (180 / M_PI)) < (180 + realAzimuthOfTowingRadians * (180 / M_PI))) {
        for (double &y : y_local) {
            y = -y;
        }
    }
    for (int i = 1; i < ChannelsVector.size(); i++) {
        double perpendicularOffsetX = y_local[i] * qCos(realAzimuthOfTowingRadians + M_PI / 2);
        double perpendicularOffsetY = y_local[i] * qSin(realAzimuthOfTowingRadians + M_PI / 2);

        ChannelsVector[i]->x_coor = x_coor + (x_local[i]) * qSin(realAzimuthOfTowingRadians + M_PI)+ perpendicularOffsetX;
        ChannelsVector[i]->y_coor = y_coor + (x_local[i]) * qCos(realAzimuthOfTowingRadians + M_PI)+ perpendicularOffsetY;
    }



    // Гарантируем совпадение первого канала с началом косы
    ChannelsVector[0]->x_coor = this->x_coor;
    ChannelsVector[0]->y_coor = this->y_coor;
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


///TODO
int Streamer::checkStreamerCoordinates()
{
    return 666;
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

double Streamer::calculateAzimuth() const {
    double dx = endBuoy->x_coor - x_coor;
    double dy = endBuoy->y_coor - y_coor;
    return atan2(dy, dx);
}





double Streamer::findA() {
    auto equation = [this](double a) {
        // return 2 * a * sinh(this->realLen / (2 * a)) - this->endBuoy->wireLength;
        return 2 * a * sinh(this->realLen / (2 * a)) - (this->endBuoy->absoluteLayback - this->absoluteLayback);
    };
    realLen = qSqrt(qPow(endBuoy->x_coor - x_coor, 2) + qPow(endBuoy->y_coor - y_coor, 2));
    double a = realLen / 2; // Начальное приближение
    double tolerance = 1e-6;
    int max_iterations = 4000;
    int iterations = 0;

    while (iterations < max_iterations) {
        double f = equation(a);

        if (f != f) {
            qDebug() << f;
        }

        double f_prime = 2 * sinh(this->realLen / (2 * a)) - (this->realLen / a) * cosh(this->realLen / (2 * a));
        if (qIsNaN(f) || qIsNaN(f_prime)) {
            qDebug() << a << f_prime;
            return -1;
        }
        // Проверка деления на ноль
        if (fabs(f_prime) < 1e-12) {
            qWarning() << "Производная слишком мала, метод Ньютона не может продолжаться. Считаем по прямой";
            return -1; // Возвращаем ошибочное значение
        }

        double delta = -f / f_prime;
        a += delta;

        if (fabs(delta) < tolerance) {
            break;
        }

        iterations++;
    }

    if (iterations == max_iterations) {
        qWarning() << "Не удалось найти параметр А за допустимое число итераций.";
        return -1; // Возвращаем ошибочное значение
    } else {
        qWarning() << QString("Удалось найти параметр А за %1 итераций.").arg(iterations);
    }

    return a;
}





