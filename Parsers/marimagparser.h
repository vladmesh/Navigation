#ifndef MARIMAGPARSER_H
#define MARIMAGPARSER_H

#include <QString>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>

class MariMagParser
{
    friend class MariMag;

    struct MagData {
        int fishID;
        float field;
        float error;
        float depth;
        float alt;
        int pitch;
        int roll;
        MagData() {};
        MagData(int fishID, float field, float error, float depth, float alt, int pitch, int roll) :
            fishID(fishID), field(field),error(error),depth(depth),alt(alt),pitch(pitch),roll(roll) {};
    };
    struct CommonMagData{
        QDateTime dateTime;
        double lat;
        double lon;
        double latc;
        double lonc;
        double altc;
        float layback;
        float baseline;
        CommonMagData() {};
        CommonMagData(QDateTime dateTime, double lat, double lon, float layback, float baseline,
                   double latc, double lonc, double altc) :
            dateTime(dateTime),lat(lat),lon(lon),
            latc(latc), lonc(lonc),altc(altc),layback(layback),baseline(baseline){};
        bool operator==(CommonMagData other) {
            return (dateTime == other.dateTime &&
            lat  == other.lat  && lon  == other.lon  &&
            layback  == other.layback  && baseline  == other.baseline  &&
            latc  == other.latc  && lonc  == other.lonc  && altc == other.altc);
        }
    };
    struct FullData{
        CommonMagData commonData;
        MagData first;
        MagData second;
    };


public:


    MariMagParser();

    static void parseMariMagFile(QString fileName);

private:
    static CommonMagData getCommonMagDataV1(QString inLine);
    static CommonMagData getCommonMagData(QString inLine);
    static MagData getMagDataV1(QString inLine);
    static FullData getFullData(std::string inLine);
    static FullData getFullData(QString inLine);


    static QString createLine(QStringList in);
    static QString createLine(FullData in);
    static void writeToOutputFile(QString line);
    static QFile* outputFile;
    static QFile* inputFile;
    static QString addSuffixToFilename(const QString& filePath, const QString& suffix);

};

#endif // MARIMAGPARSER_H
