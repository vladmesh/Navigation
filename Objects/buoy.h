#ifndef BUOY_H
#define BUOY_H

#include "toweditem.h"

/**
 * @class Buoy
 * @brief Представляет объект "Буй"
 *
 * Класс `Buoy` является наследником `TowedItem` и специализирован для реализации функциональности,
 * связанной с буями. Имеет доп.функционал для управления положением каналов косы к которой прикреплен
 */
class Buoy : public TowedItem
{
    Q_OBJECT
    friend class Fabric;
    friend class ItemsLoader;

    friend class testPostprocessing;
    /**
     * @brief Конструктор с параметрами для создания буя.
     * @param Name Имя буя.
     * @param towingPoint Указатель на точку буксировки.
     * @param angleToWired Угол между направлением буксировки и объектом (в градусах).
     * @param wireLength Длина троса (в метрах).
     * @param AnthenaHeight Высота антенны (в метрах).
     * @param towingDepth Глубина буксировки (в метрах).
     */
    Buoy(QString Name,
         FixedItem* towingPoint, double angleToWired,
         double wireLength,double AnthenaHeight, double towingDepth);
public:
    Buoy();
    ~Buoy();

    int buoyID;

    double AnthenaHeight = 1; ///< Высота антенны (в метрах).
    double towingDepth = 1;   ///< Глубина буксировки (в метрах).
};

#endif // BUOY_H
