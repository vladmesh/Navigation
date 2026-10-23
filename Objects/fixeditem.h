#ifndef FIXEDITEM_H
#define FIXEDITEM_H

#include <QString>
#include <QDebug>
#include <QVector>
#include <QDebug>
#include <QObject>
#include <QScopedPointer>
#include <QPointer>

#include "nmeaparser.h"
// #include "connection.h"
#include "connectionlogic.h"
// #include "logger.h"
#include "CircularBuffer.h"

/**
 * @class FixedItem
 * @brief Представляет фиксированный элемент (например, объект на судне) с привязкой к координатам и соединениям.
 * От него наследуются другие классы, но метод calcIfNotConnected меняется
 * Класс `FixedItem` содержит информацию о объекте, его географических и UTM-координатах,
 * а также предоставляет методы для расчетов на основе данных NMEA, связи с другими объектами и работы с соединениями.
 */
class FixedItem : public QObject
{
Q_OBJECT
private:
    friend class Fabric;
    // friend class Coordinator;
    friend class ItemsLoader;
    friend class MariMag;
    /**
     * @brief Конструктор с параметрами для создания фиксированного объекта.
     * @param x Начальная координата X на схеме.
     * @param y Начальная координата Y на схеме.
     * @param z Не используется
     * @param name Имя объекта.
     */
    FixedItem(double x,double y,double z, QString name);
    /**
     * @brief Вычисляет координаты объекта, если он имеет соединение.
     */
    void calcIfConnected();
    /**
     * @brief Вычисляет координаты объекта, если он не имеет соединение.
     * База рассчета - объект с соединением
     */
    virtual void calcIFNotConnected();


public:
    FixedItem();
    virtual ~FixedItem();

    /**
     * @brief Основной метод вычисления координат объекта.
     */
    void calcItemCoordinates();

    /// Координаты объекта на схеме.
    double x; ///< Координата X на схеме.
    double y; ///< Координата Y на схеме.
    double z; ///< Высота объекта на схеме.

    /// Реальные координаты объекта.
    double x_coor = 0; ///< Координата X в UTM.
    double y_coor = 0; ///< Координата Y в UTM.
    double latitude = 0; ///< Географическая широта.
    double longitude = 0; ///< Географическая долгота.
    double height; ///< Высота объекта.
    double azimuthOfMovement; ///< Азимут движения объекта.
    /// Последние полученные данные NMEA.
    NmeaParser::CoordinateData lastGGAData = {{0, 0}, {0, 0}, QDateTime(), 0}; ///< Данные NMEA GGA.
    NmeaParser::NmeaRMCData lastRMCData = {0, 0}; ///< Данные NMEA RMC.

    /**
     * @brief Выводит текущие координаты объекта. Для отладки.
     */
    void printPos();


    /// Информация об объекте.
    QString name; ///< Имя объекта.
    QString itemType; ///< Тип объекта.
    int number; ///< Идентификатор объекта (не используется).

    ///funcs for working with connected items
    /**
     * @brief Удаляет все связи объекта с другими объектами.
     */
    void deletingWires();

    /**
     * @brief Обрабатывает событие потери связи объекта.
     */
    void lostWire();
    /**
     * @brief Получает текущие координаты объекта как точку.
     * @return Координаты объекта.
     */
    QPointF getCoordinates();

    ///funcs for working with connections
    /**
     * @brief Получает последнюю строку NMEA GGA.
     * @return Строка NMEA GGA.
     */
    QString getLastGGA();

    /**
     * @brief Получает последнюю строку NMEA RMC.
     * @return Строка NMEA RMC.
     */
    QString getLastRMC();

    /// Поля для управления подключением.
    bool hasConnection = false; ///< Флаг наличия подключения. Может меняться в зависимости от состояниея подключения
    QPointer<Connection> connection; ///< Указатель на соединение.

    NmeaParser nmeaParser;
    QVector<QPointer<FixedItem>> vectorOfConnected = {}; ///< Список подключенных объектов.

    /**
     * @brief Устанавливает объект для вычислений координат.
     * @param newItemForCalculations Указатель на объект для вычислений.
     */
    void setItemForCalculations(FixedItem* newItemForCalculations);

    static double realAzimuthOfTowingRadians; ///< Общий азимут для расчета буксировки.

    /**
     * @brief метод для отладки. выводит азимут и расстояние до другого объекта
     * @param to - до кого, собстна
     * @return всегда true
     */
    bool showAzDistToObj(FixedItem* to);

    int creationPriority = -1;///< Приоритет создания.
    //присядь путник, я тебе расскажу про легендарный костыль
    //давным давно, была нужда создававать такие объекты на основе JSON возникла проблема
    //ведь многие объекты имели в себе ссылки на другие объекты
    //и придуман был приоритет порядка создания, что разрешал этот вопрос
    //но как реализовать этот механизм?
    //1. можно через статик метод хранилища, что сортирует все имеющиеся объекты по а)типу
    //б)наличию соединений в)последовательностью подключения (

    int calculationPriority = -1;///< Приоритет вычислений.

    bool needCalculate = true; ///<флаг для того, чтобы в случае чего останаливать расчеты
    bool amIItemForCalculating = false; ///< Флаг использования объекта для расчетов.

    FixedItem* ItemForCalculations = nullptr; ///< Указатель на объект для расчетов.
signals:
    /**
     * @brief Сигнал изменения состояния подключения.
     * @param who Указатель на себя.
     */
    void hasConnectionChanged(QPointer<FixedItem> who);


public slots:
    /**
     * @brief Слот, вызываемый при изменении состояния подключения.
     * @param newHasConnection Новое состояние подключения.
     */
    void hasConnectionArrived(bool newHasConnection);

protected:
    static CircularBuffer sharedCircularBuffer; ///< Общий кольцевой буфер координат.

};




#endif // FIXEDITEM_H
