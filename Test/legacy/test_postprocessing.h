#ifndef TEST_POSTPROCESSING_H
#define TEST_POSTPROCESSING_H

#include "postprocessor.h"
#include "fabric.h"
#include "itemsstorage.h"
#include "segyreader.h"

#include <QTime>
#include <QThread>

class testPostprocessing
{

public:
    testPostprocessing();

    double getDistance(NmeaParser::CoordinateData prev, NmeaParser::CoordinateData next);

    int testSearchingNmea23();
    int testSearchingNmea23WithOnboard();
    int testSearchingNmea22();
    int testSearchingNmea23EndBuoyOnlyFans();
    void testPpkFindString();
    void testCoorCorrection();
    void testChainAlgo();


    QVector<FixedItem*> createItems2309EndBuoyOnlyFans();
    QVector<FixedItem*> createItems2309WithOnboard();
    QVector<FixedItem*> createItemsСhainAlgo();
    QVector<FixedItem*> createItems2209();

    QVector<FixedItem*> testItems;
    QVector<SegYReader::SegyData>readFileAndGeneratePairs(const QString& fileName);

    double shortestDistanceToLine(const QPointF& point, const QPointF& lineStart, const QPointF& lineEnd);

    // void

    QDate getDateFromDayOfYear(int dayOfYear) {
        // Проверяем, что номер дня находится в допустимом диапазоне
        if (dayOfYear < 1 || dayOfYear > 366) {
            qWarning() << "Номер дня должен быть от 1 до 366.";
            return QDate(); // Возвращаем некорректную дату
        }
        int year = QDate::currentDate().year();
        // Получаем первую дату года
        QDate firstDayOfYear(year, 1, 1);

        // Возвращаем дату, прибавляя номер дня минус один
        QDate ret = firstDayOfYear.addDays(dayOfYear-1);
        return ret;
    }


    void setPath(P190_creator* victim);
};

#endif // TEST_POSTPROCESSING_H
