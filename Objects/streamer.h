#ifndef STREAMER_H
#define STREAMER_H

#include "toweditem.h"
#include "fixeditem.h"
#include "buoy.h"

#include <QObject>

/**
 * @class Streamer
 * @brief Представляет сейсмическую косу, являющуюся буксируемым объектом.
 *
 * Streamer наследует функциональность TowedItem и расширяет её для работы с каналами косы,Основные задачи класса:
 * - Вычисление координат каналов.
 * - Управление глубиной каналов и общей длиной косы.
 * - Взаимодействие с концевой точкой косы, например, буйем.
 * - Обработка различных конфигураций косы, включая шаг каналов, фиксированное количество каналов или индивидуальный тип канала.
 */
class Streamer : public TowedItem
{
    Q_OBJECT

    /**
     * @class Channel
     * @brief Внутренний класс, представляющий отдельный канал сейсмической косы.
     *
     * Каждый канал хранит свою позицию (UTM), глубину и номер.
     */
    class Channel;


    friend class Fabric;
    friend class ItemsLoader;
    friend class testPostprocessing;
    /// @brief Конструктор для создания косы с заданными каналами.
    Streamer(QString name, FixedItem *towingPoint, double angleToWired, double absoluteLayback, uint NumChannels, QVector<double> chans);
    /// @brief Конструктор для создания косы с заданным шагом каналов.
    Streamer(QString name, FixedItem *towingPoint, double angleToWired, double absoluteLayback, uint NumChannels, QString chanStep);
    /// @brief Конструктор для создания одноканальной косы
    Streamer(QString name, FixedItem *towingPoint, double angleToWired, double absoluteLayback, QString singleChannelType);

public:

    explicit Streamer(QObject *parent = nullptr);
    /// @brief Вычисляет координаты каналов косы.
    void calcChansCoors();
     /// @brief Вычисляет координаты каналов, если конечный буй отсутствует.
    void calcChansCoorsNoEndBuoy();

    void calcChansStreamerIsStrict();

    /// @brief Печатает координаты каналов. Для отладки.
    void printChansCoor();

    /// @brief Возвращает указатель на канал по его номеру.
    Channel* getChan(uint number);

    /// @brief Возвращает количество каналов.
    uint getChanCount();


    /// @brief Устанавливает конечный буй косы.
    void setEndBuoy(Buoy *newEndBuoy);


    /// @brief Глубина первого канала в косе.
    double depth;

    /// @brief Вычисляет глубину каждого канала.
    void calcStreamerDepth();

    double totalLength;
    Buoy* endBuoy = nullptr;


    /// @brief Шаг между каналами, заданный строкой.
    QString channelStep = "1";
    /// @brief Константа для установки глубины по умолчанию.
    static double defaultChannelDepth;

    /// @brief Тип одного канала (например, "1x8" или "1x16").
    QString singleChannelType = "";

    int streamerID = -1;


    QVector<double> chans; ///< Относительные позиции каналов.
private:
    uint NumChanels; ///< Количество каналов в косе.
    QVector<Channel*> ChannelsVector; ///< Вектор каналов (объектов Channel).


    double realLen; ///< Длина от начала до концевого буя.
    double dh; ///< Изменение высоты по длине косы.
    double distanceCalcCoef; ///< Коэффициент расчета длины.


    /// @class Channel
    /// @brief Представляет отдельный канал косы.
    class Channel : public TowedItem
    {
    public:
        Channel(uint myNumber);

        QString getUTMPos();
        double depth;

    private:
        uint myNumber;
    };


};


#endif // STREAMER_H
