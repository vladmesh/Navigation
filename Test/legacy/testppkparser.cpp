#include "testppkparser.h"
#include "testdatapath.h"

testPpkParser::testPpkParser() {

    readPpk();
}

void testPpkParser::readPpk()
{
    QString pathPPK = testDataPath("Desktop/FINAL/PPK GNSS data/test/GD_buoy_PPK_20240922043218.txt");
    QString pathNMEA = testDataPath("Desktop/FINAL/PPK GNSS data/test/22092024endBuoy.nmea");
    QFile filePPKStorage(pathPPK);
    QFile fileNMEAStorage(pathNMEA);
    QFile* filePPK = &filePPKStorage;
    QFile* fileNMEA = &fileNMEAStorage;
    if (!filePPK->open(QIODevice::ReadOnly) || !fileNMEA->open(QIODevice::ReadOnly)) {
        qWarning() << "testPpkParser: test data not found:" << pathPPK << pathNMEA;
        return;
    }

    QString linePPK;
    QString lineNMEA;
    NmeaParser nmeaparser;
    while (!filePPK->atEnd() && !fileNMEA->atEnd()) {

        linePPK = filePPK->readLine();
        lineNMEA = fileNMEA->readLine();
        NmeaParser::CoordinateData ppkdata = PpkParser::parseLine(linePPK );
        NmeaParser::CoordinateData nmeadata = nmeaparser.parseNmeaGGA(lineNMEA);
        if (PpkParser::compareCoordinates(ppkdata, nmeadata )) {
        } else {
            qDebug() << "NOGOOD";
            qDebug() << ppkdata.coordinate << ppkdata.coorUTM << ppkdata.dateTime << ppkdata.height;
            qDebug() << nmeadata.coordinate << nmeadata.coorUTM << nmeadata.dateTime << nmeadata.height;
        }
    }

}
