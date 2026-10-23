#include "testppkparser.h"

testPpkParser::testPpkParser() {

    readPpk();
}

void testPpkParser::readPpk()
{
    QString pathPPK = "C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/test/GD_buoy_PPK_20240922043218.txt";
    QString pathNMEA = "C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/test/22092024endBuoy.nmea";
    QFile* filePPK = new QFile(pathPPK);
    QFile* fileNMEA = new QFile(pathNMEA);
    filePPK->open(QIODevice::ReadOnly);
    fileNMEA->open(QIODevice::ReadOnly);

    QString linePPK;
    QString lineNMEA;
    NmeaParser nmeaparser;
    while (!filePPK->atEnd() && !fileNMEA->atEnd()) {

        linePPK = filePPK->readLine();
        lineNMEA = fileNMEA->readLine();
        NmeaParser::CoordinateData ppkdata = PpkParser::parseLine(linePPK );
        NmeaParser::CoordinateData nmeadata = nmeaparser.parseNmeaGGA(lineNMEA);
        // exit(123);
        if (PpkParser::compareCoordinates(ppkdata, nmeadata )) {
            // qDebug() << "GOOD";
        } else {
            qDebug() << "NOGOOD";
            qDebug() << ppkdata.coordinate << ppkdata.coorUTM << ppkdata.dateTime << ppkdata.height;
            qDebug() << nmeadata.coordinate << nmeadata.coorUTM << nmeadata.dateTime << nmeadata.height;
        }
        // break;
    }

}
