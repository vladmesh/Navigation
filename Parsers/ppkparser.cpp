#include "ppkparser.h"


PpkParser::PpkParser(QObject *parent)
    : NmeaParser{parent}
{

}

double PpkParser::dmsToDecimal(int degrees, int minutes, double seconds)
{
    return degrees + (minutes / 60.0) + (seconds / 3600.0);
}

NmeaParser::CoordinateData PpkParser::parseLine(const QString &line) {
    CoordinateData result;

    // Split the line by spaces (with trimming to handle multiple spaces correctly)
    if (line.isEmpty()) return result;
    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (parts.size() != 28) return result;
    if (parts[0] == "DIR") return result;

    // Extract individual values
    int latDeg = parts[16].toInt();
    int latMin = parts[17].toInt();
    double latSec = parts[18].toDouble();

    int lonDeg = parts[19].toInt();
    int lonMin = parts[20].toInt();
    double lonSec = parts[21].toDouble();

    double height = parts[22].toDouble();
    double utmEasting = parts[24].toDouble();
    double utmNorthing = parts[25].toDouble();

    QString dateTimeStr = parts[4] + " " + parts[5]; // Combine date and time
    result.dateTime = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd hh:mm:ss.zz");

    // Convert lat/lon to decimal degrees and create QGeoCoordinate
    double latitude = dmsToDecimal(latDeg, latMin, latSec);
    double longitude = dmsToDecimal(lonDeg, lonMin, lonSec);
    result.coordinate = QGeoCoordinate(latitude, longitude);

    // Store UTM coordinates in QPointF
    result.coorUTM = QPointF(utmEasting, utmNorthing);

    // Store height
    result.height = static_cast<float>(height);

    return result;
}

bool PpkParser::compareCoordinates(CoordinateData nmea, CoordinateData ppk)
{
    if (nmea.coorUTM != ppk.coorUTM) {
        if (abs(nmea.coorUTM.x() - ppk.coorUTM.x()) > 0.01 || abs(nmea.coorUTM.y() - ppk.coorUTM.y()) > 0.01) {
            qDebug() << nmea.coorUTM.x() - ppk.coorUTM.x() << nmea.coorUTM.y() - ppk.coorUTM.y();
            return false;
        }
    }
    if (nmea.coordinate != ppk.coordinate) {
        if (abs(nmea.coordinate.latitude() - ppk.coordinate.latitude()) > 0.01 || abs(nmea.coordinate.longitude() - ppk.coordinate.longitude()) > 0.01) {
            qDebug() << nmea.coordinate.latitude() - ppk.coordinate.latitude() << nmea.coordinate.longitude() - ppk.coordinate.longitude();
            return false;
        }
    }
    if (nmea.dateTime.time() != ppk.dateTime.time()) {
        qDebug() << nmea.dateTime.time() << ppk.dateTime.time();
        return false;
    }
    return true;
}

QTime PpkParser::getTimeFromPpkLine(const QString &PpkSentence)
{
    QDateTime result = QDateTime();
    QStringList parts = PpkSentence.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    // if (parts.size() == 0) return result.time();
    if (parts.size() != 28) return result.time();
    if (parts[0] == "DIR") return result.time();
    QString timeStr = parts[5]; // Combine date and time
    return QTime::fromString(timeStr, "hh:mm:ss.zz");
    // return result.time();
}









