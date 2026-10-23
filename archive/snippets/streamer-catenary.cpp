// Archived from Objects/streamer.cpp / streamer.h. Commit: 4ac56dd (Initial import), identical in main aee8ae8.
// Feature: streamer channel layout along a catenary (chain line) between the
// streamer head and the tail buoy, instead of a straight line.
// Status: disabled in main by `#define CHAINALGO` placed right before
// `#ifdef CHAINALGO`, so calcChansStreamerIsStrict() was always called.
// Known issues: `realAzimuthOfTowingRadians + M_PI/2;` has no effect (no
// assignment); comment "пока не используется". Manual scenario
// testPostprocessing::testChainAlgo (Test/legacy) exercised it on field data.
// To restore: add the declarations below to class Streamer and call
// calcChansByChainAlgorithm() from calcChansCoors() when the end buoy has a
// connection.
//
// Declarations (streamer.h):
//     /// @brief Вычисляет координаты каналов алгоритмом цепи.
//     void calcChansByChainAlgorithm();
// protected:
//     double findA();
//     double calculateAzimuth() const;   // azimuth head -> end buoy, unused
//
// Dispatch that was in Streamer::calcChansCoors():
// #define CHAINALGO
// #ifdef CHAINALGO
//             calcChansStreamerIsStrict();
// #else
//             calcChansByChainAlgorithm();
// #endif

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

double Streamer::calculateAzimuth() const {
    double dx = endBuoy->x_coor - x_coor;
    double dy = endBuoy->y_coor - y_coor;
    return atan2(dy, dx);
}
