#ifndef TOWEDITEM_H
#define TOWEDITEM_H

#include "fixeditem.h"
#include <QtMath>

/**
 * @class TowedItem
 * @brief Представляет абстрактный буксируемый объект, связанный с точкой буксировки (например, судном или другим объектом).
 *
 * Класс `TowedItem` используется для представления буксируемого объекта, вычисления координат на основе
 * точки буксировки и анализа движения.
 * Наследует функциональность класса `FixedItem` и добавляет особенности, связанные с буксировкой.
 */
class TowedItem : public FixedItem
{
Q_OBJECT
private:
    friend class Fabric;
    friend class Streamer;
    friend class Buoy;
    friend class Source;
    friend class ItemsLoader;
    /**
     * @brief Конструктор с параметрами для создания буксируемого объекта.
     * @param name Имя буксируемого объекта.
     * @param towingPoint Указатель на точку буксировки.
     * @param angleToWired Угол между направлением буксировки и объектом (в градусах).
     * @param absoluteLayback Длина до точки буксировки.
     */


    TowedItem(QString name, FixedItem* towingPoint,
              double angleToWired, double absoluteLayback);
public:
    TowedItem();
    virtual ~TowedItem();

    void updateLayout();
    // void calcItemCoordinates();
    /**
     * @brief Перегруженный метод вычисления координат объекта, если он не подключен.
     */
    void calcIFNotConnected() override;

    /**
     * @brief Выводит информацию об объекте.
     */
    void printSelfInfo();

    /// Поля для буксировки.
    FixedItem* towingPoint = nullptr; ///< Указатель на точку буксировки.
    double angle; ///< Угол между направлением буксировки и объектом.
    double wireLength; ///< Длина буксировочного троса.
    double absoluteLayback = 0;

    /**
     * @brief Возвращает последнюю строку NMEA GGA, связанную с объектом.
     * @return Строка NMEA GGA.
     */
    QString getLastGGA();

    /**
     * @brief Возвращает последнюю строку NMEA RMC, связанную с объектом.
     * @return Строка NMEA RMC.
     */
    QString getLastRMC();

    QVector<FixedItem*> vectorOfConnections; ///< Список подключенных объектов
    // QString name;
    // QString itemType;
    /**
     * @brief Устанавливает высоту борта объекта. Больше не используется.
     * @param newBoardHeight Новая высота борта.
     */
    void setBoardHeight(double newBoardHeight);

    /**
     * @brief Выводит текущие координаты и информацию об объекте.
     */
    void printPos();


    void setWireLength(double newWireLength);

protected:
    double boardHeight = 0;///< Высота борта объекта относительно базовой точки. Не используется


};

#endif // TOWEDITEM_H
