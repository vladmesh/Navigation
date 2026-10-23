#ifndef NMEAPARSER_H
#define NMEAPARSER_H

#include <QObject>
#include <QGeoCoordinate>
#include <QTime>
#include <QDateTime>
#include <QString>
#include <QPointF>
#include <QMessageBox>

/**
 * @class NmeaParser
 * @brief Класс для парсинга и обработки данных NMEA.
 *
 * Обеспечивает функции для парсинга NMEA-строк, преобразования координат между
 * географическими (широта/долгота) и UTM и обратно, а также проверки корректности данных.
 */
class NmeaParser : public QObject
{
    Q_OBJECT
public:
    enum NmeaType{
        GGA,
        RMC
    };
    /**
     * @struct CoordinateData
     * @brief Структура для хранения данных из строки GGA.
     */
    struct CoordinateData {
        QGeoCoordinate coordinate;
        QPointF coorUTM;
        QDateTime dateTime;
        float height = 0.0f;
    };
    /**
     * @struct NmeaRMCData
     * @brief Структура для хранения данных из строки RMC.
     */
    struct NmeaRMCData{
        double azimuth;
        double speed;
    };

    explicit NmeaParser(QObject *parent = nullptr);
    ~NmeaParser() {};

    /**
     * @brief Извлекает время из строки NMEA GGA.
     * используется в постпроцессинге, и работает быстрее полного парсинга строки и извлечения всех данных
     * @param nmeaSentence Строка NMEA GGA.
     * @return Время в формате `QTime`.
     */
    QTime getTimeFromNmeaGGA(const QString &nmeaSentence);
    /**
     * @brief Парсит строку NMEA GGA.
     * @param nmeaSentence Строка NMEA GGA.
     * @return Структура с координатами, временем и высотой.
     */
    CoordinateData parseNmeaGGA(const QString &nmeaSentence);
    /**
     * @brief Парсит строку NMEA RMC.
     * @param nmeaSentence Строка NMEA RMC.
     * @return Структура с азимутом и скоростью.
     */
    NmeaRMCData parseNmeaRMC(const QString &nmeaSentence);
    /**
     * @brief Преобразует географические координаты в UTM. Заодно сохраняет номер зоны
     * @param coordinate Географические координаты (широта, долгота).
     * @return Координаты в системе UTM.
     */
    QPointF GeoToUTM(const QGeoCoordinate &coordinate);
    /**
     * @brief Преобразует UTM координаты в географические.
     * @param coordinate Координаты в системе UTM.
     * @return Географические координаты.
     */
    QGeoCoordinate UTMtoGeo(const QPointF &coordinate);


    /**
     * @brief Проверяет корректность данных из строки GGA.
     * @param nmeaGGA Данные GGA.
     * @return `true`, если данные корректны.
     */
    static bool isValid(CoordinateData nmeaGGA);
    /**
     * @brief Проверяет корректность данных из строки RMC.
     * @param nmeaRMC Данные RMC.
     * @return `true`, если данные корректны.
     */
    static bool isValid(NmeaRMCData nmeaRMC);
private:
    /**
     * @brief Конвертирует координаты NMEA в градусы.
     * @param nmeaValue Значение NMEA в формате градусы+минуты.
     * @param direction Направление (N, S, E, W).
     * @return Значение в градусах.
     */
    double convertToDegrees(const QString &nmeaValue, const QString &direction);

    bool isNorth = true; ///< Флаг для указания северного полушария.

    //constants for calculation
    static int ZoneNumber; ///< Номер UTM-зоны.
    const double k0 = 0.9996;///< Масштабный коэффициент.
    const double a = 6378137.0;  ///< Большая полуось WGS84.
    const double eccSquared = 0.00669438;  ///< Квадрат эксцентриситета WGS84.
    const double eccPrimeSquared = eccSquared / (1 - eccSquared);///< Скорректированный эксцентриситет.
    const double e1 = (1 - sqrt(1 - eccSquared)) / (1 + sqrt(1 - eccSquared));///< Вспомогательный параметр.
};

#endif // NMEAPARSER_H
