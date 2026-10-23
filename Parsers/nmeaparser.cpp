#include "nmeaparser.h"


int NmeaParser::ZoneNumber = 0;

NmeaParser::NmeaParser(QObject *parent)
    : QObject{parent}
{

}

QTime NmeaParser::getTimeFromNmeaGGA(const QString &nmeaSentence)
{
    QTime time(-1,-1,-1,-1);
    if (nmeaSentence.startsWith("$GPGGA") || nmeaSentence.startsWith("$GNGGA")) {
        QStringList parts = nmeaSentence.split(',');
        if (parts.size() > 9) {
            QString timeValue = parts[1];
            int hours = timeValue.mid(0, 2).toInt();
            int minutes = timeValue.mid(2, 2).toInt();
            int seconds = timeValue.mid(4, 2).toInt();
            int milliseconds = timeValue.mid(7, 2).toInt()*10;
            if (QTime::isValid(hours, minutes, seconds, milliseconds)) {
                time = QTime(hours, minutes, seconds, milliseconds);
            }
        }
    }
    // qDebug() << time << __FUNCTION__;
    return time;
}

NmeaParser::CoordinateData NmeaParser::parseNmeaGGA(const QString &nmeaSentence) { //разбиваем NMEA на части
    CoordinateData data;

    if (nmeaSentence.startsWith("$GPGGA") || nmeaSentence.startsWith("$GNGGA")) {
        QStringList parts = nmeaSentence.split(',');

        if (parts.size() > 9) {
            QString timeValue = parts[1];
            QString latitudeValue = parts[2];
            QString latitudeDirection = parts[3];
            QString longitudeValue = parts[4];
            QString longitudeDirection = parts[5];
            QString heightValue = parts[9];



            double latitude = convertToDegrees(latitudeValue, latitudeDirection); //получение географических градусов
            double longitude = convertToDegrees(longitudeValue, longitudeDirection); //в норм формате

            float height = heightValue.toFloat();

            int hours = timeValue.mid(0, 2).toInt();
            int minutes = timeValue.mid(2, 2).toInt();
            int seconds = timeValue.mid(4, 2).toInt();
            int milliseconds = timeValue.mid(7, 2).toInt()*10;
            QTime time(hours, minutes, seconds, milliseconds);
            QDateTime dateTime(QDateTime::currentDateTime().date(), time);



            data.coordinate = QGeoCoordinate(latitude, longitude);

            // qDebug()<<latitudeValue<<longitudeValue<<"_____________";
            // qDebug()<<data.coordinate.latitude()<<data.coordinate.longitude()<<"_____________";

            QPointF coorUTM= GeoToUTM(data.coordinate);
            data.coorUTM = coorUTM;
            data.dateTime = dateTime;
            data.height = height;
        }
    }
    // qDebug() << data.coordinate.latitude() << data.coordinate.longitude();
    return data;
}

NmeaParser::NmeaRMCData NmeaParser::parseNmeaRMC(const QString &nmeaSentence)
{
    NmeaRMCData rmcData {-1,-1};
    if (nmeaSentence.startsWith("$GPHDT")) {
        // rmcData.azimuth = nmeaSentence.mid(7,4).toFloat();
        rmcData.azimuth = nmeaSentence.mid(nmeaSentence.indexOf(',')+1).toFloat();
        // qDebug() << __FUNCTION__ << rmcData.azimuth;
        return rmcData;
    }

    // Проверяем, что строка начинается с "$GPRMC"
    if (!nmeaSentence.startsWith("$GPRMC") && !nmeaSentence.startsWith("$GNRMC")) {
        qDebug() << "Error: Invalid NMEA RMC string format.";
        qDebug() << nmeaSentence;
        return rmcData;
    }

    // Разбиваем строку по запятым
    QStringList parts = nmeaSentence.split(',');
    // qDebug() << "parseNmeaRMC" <<parts;

    // Проверяем, что у нас достаточно частей для извлечения данных
    if (parts.size() < 8) {
        qDebug() << "Error: Insufficient data fields in NMEA RMC string.";
        return rmcData;
    }

    // Извлекаем данные
    bool azimuthOk, speedOk;
    rmcData.azimuth = parts[8].toDouble(&azimuthOk); // Курс над землей
    rmcData.speed = parts[7].toDouble(&speedOk);    // Скорость над землей в узлах

    // Проверяем успешность извлечения данных
    if (!azimuthOk || !speedOk) {
        qDebug() << "Error: Failed to parse azimuth or speed from NMEA RMC string.";
        rmcData.azimuth = -1;
        rmcData.speed = -1;
    }

    return rmcData;

}

double NmeaParser::convertToDegrees(const QString &nmeaValue, const QString &direction) { //перевод координат из градусы+минуты в градусы+доли градуса
    if (nmeaValue.isEmpty() || direction.isEmpty()) {
        return 0.0;
    }

    double degrees;
    double minutes;
    double decimalDegrees;
    bool ok;

    if (direction == "S" || direction == "N" ) {
        degrees = nmeaValue.left(2).toDouble(&ok);
        minutes = nmeaValue.mid(2).toDouble(&ok);
        decimalDegrees = degrees + (minutes / 60.0);
        if (!ok) return 0.0;
    } else {
        degrees = nmeaValue.left(3).toDouble(&ok);
        minutes = nmeaValue.mid(3).toDouble(&ok);
        decimalDegrees = degrees + (minutes / 60.0);
        if (!ok) return 0.0;
    }

    if (direction == "S" || direction == "W") {
        decimalDegrees = -decimalDegrees;
    }

    return decimalDegrees;
}

void NmeaParser::printNmeaGGAData(CoordinateData data) {
    if (data.coordinate.isValid()) {
        qDebug() << "Широта:" << data.coordinate.latitude();
        qDebug() << "Долгота:" << data.coordinate.longitude();
        qDebug() << "Время:" << data.dateTime.toString();
        qDebug() << "Высота:" << data.height;

        qDebug() << "UTM Восток:" << data.coorUTM.x();
        qDebug() << "UTM Север:" << data.coorUTM.y();
        qDebug() << "Высота над уровнем моря"<< data.height;
    } else {
        qDebug() << "Некорректная строка NMEA GGA";
    }
}

void NmeaParser::printNmeaRMCData(NmeaRMCData data)
{
    if (data.azimuth >= 0 && data.azimuth <= 360 && data.speed > 0) {
        qDebug() << "Скорость" << data.speed;
        qDebug() << "Направление" << data.azimuth;
    } else {
        qDebug() << "Некорректная строка NMEA RMC";
    }
}

QPointF NmeaParser::GeoToUTM(const QGeoCoordinate &coordinate) { //перевод географических координат в систему проекции
    double N, T, C, A, M;
    double LatRad = coordinate.latitude() * M_PI / 180.0;
    double LongRad = coordinate.longitude() * M_PI / 180.0;
    double LongOriginRad;
    ZoneNumber = std::floor((coordinate.longitude() + 180) / 6) + 1;
    // qDebug() << "GeoToUTM zone number " << ZoneNumber;
    LongOriginRad = ((ZoneNumber - 1) * 6 - 180 + 3) * M_PI / 180.0;
    N = a / std::sqrt(1 - eccSquared * std::sin(LatRad) * std::sin(LatRad));
    T = std::tan(LatRad) * std::tan(LatRad);
    C = eccPrimeSquared * std::cos(LatRad) * std::cos(LatRad);
    A = std::cos(LatRad) * (LongRad - LongOriginRad);

    M = a * ((1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64 - 5 * eccSquared * eccSquared * eccSquared / 256) * LatRad
             - (3 * eccSquared / 8 + 3 * eccSquared * eccSquared / 32 + 45 * eccSquared * eccSquared * eccSquared / 1024) * std::sin(2 * LatRad)
             + (15 * eccSquared * eccSquared / 256 + 45 * eccSquared * eccSquared * eccSquared / 1024) * std::sin(4 * LatRad)
             - (35 * eccSquared * eccSquared * eccSquared / 3072) * std::sin(6 * LatRad));

    double UTMEasting = (k0 * N * (A + (1 - T + C) * A * A * A / 6
                                   + (5 - 18 * T + T * T + 72 * C - 58 * eccPrimeSquared) * A * A * A * A * A / 120)
                         + 500000.0);

    double UTMNorthing = (k0 * (M + N * std::tan(LatRad) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24
                                                            + (61 - 58 * T + T * T + 600 * C - 330 * eccPrimeSquared) * A * A * A * A * A * A / 720)));

    if (coordinate.latitude() < 0) {
        isNorth = false;
        UTMNorthing += 10000000.0;
    } else {
        isNorth = true;
    }

    return QPointF(UTMEasting, UTMNorthing);
}

QGeoCoordinate NmeaParser::UTMtoGeo(const QPointF &coordinate) { //из utm в географические
    double easting = coordinate.x();
    double northing = coordinate.y();
    double x = easting - 500000.0;  // Убрать 500,000 меток, для приведения к центру зоны
    double y = isNorth ? northing : northing - 10000000.0;  // Убрать 10,000,000 меток, если это южное полушарие

    double m = y / k0;
    double mu = m / (a * (1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64 - 5 * eccSquared * eccSquared * eccSquared / 256));

    double phi1Rad = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu)
                     + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu)
                     + (151 * e1 * e1 * e1 / 96) * sin(6 * mu);

    double n1 = a / sqrt(1 - eccSquared * sin(phi1Rad) * sin(phi1Rad));
    double t1 = tan(phi1Rad) * tan(phi1Rad);
    double c1 = eccPrimeSquared * cos(phi1Rad) * cos(phi1Rad);
    double r1 = a * (1 - eccSquared) / pow(1 - eccSquared * sin(phi1Rad) * sin(phi1Rad), 1.5);
    double d = x / (n1 * k0);

    double lat = phi1Rad - (n1 * tan(phi1Rad) / r1) * (d * d / 2 - (5 + 3 * t1 + 10 * c1 - 4 * c1 * c1 - 9 * eccPrimeSquared) * d * d * d * d / 24
        + (61 + 90 * t1 + 298 * c1 + 45 * t1 * t1 - 252 * eccPrimeSquared - 3 * c1 * c1) * d * d * d * d * d * d / 720);
    lat = lat * (180.0 / M_PI);

    double lon = (d - (1 + 2 * t1 + c1) * d * d * d / 6
                  + (5 - 2 * c1 + 28 * t1 - 3 * c1 * c1 + 8 * eccPrimeSquared + 24 * t1 * t1) * d * d * d * d * d / 120) / cos(phi1Rad);
    lon = ZoneNumber > 0 ? lon * (180.0 / M_PI) + (ZoneNumber * 6 - 183) : lon * (180.0 / M_PI);

    // qDebug() << "UTMtoGeo";
    // qDebug() << "Easting:" << easting << "Northing:" << northing;
    // qDebug() << "Zone:" << ZoneNumber << "isNorth:" << isNorth;
    // qDebug() << "x:" << x << "y:" << y;
    // qDebug() << "m:" << m << "mu:" << mu;
    // qDebug() << "phi1Rad:" << phi1Rad;
    // qDebug() << "lat:" << lat << "lon:" << lon;
    return QGeoCoordinate(lat, lon);
}

bool NmeaParser::isValid(CoordinateData nmeaGGA) {
    int res = 1;
    if (nmeaGGA.coorUTM.rx() == 0 || nmeaGGA.coorUTM.ry() == 0) res = 0;
    if (!nmeaGGA.dateTime.isValid()) res = 0;
    return res;
}

bool NmeaParser::isValid(NmeaRMCData nmeaRMC) {
    int res = 1;
    if (nmeaRMC.azimuth < 0 || nmeaRMC.azimuth >= 360) res = 0;
    if (nmeaRMC.speed < 0 || nmeaRMC.speed > 50) res = 0;
    return res;
}









