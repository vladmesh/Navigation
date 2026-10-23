#ifndef SOURCE_H
#define SOURCE_H

#include <QObject>
#include "toweditem.h"

/**
 * @class Source
 * @brief Представляет источник (например,спаркер или бумер), буксируемый другим объектом.
 * Класс `Source` наследует функциональность `TowedItem`.
 * P190 по-особому работает с этим классом
 */
class Source : public TowedItem
{
    friend class Fabric;
    Q_OBJECT
public:
    Source();
    Source(QString Name,
           FixedItem* towingPoint = nullptr, double angle = 0, double wireLength = 5,
           double AnthenaHeight = 0, double sourceDepth = 0);
    int sourceID = -1;
    double AnthenaHeight = 1; ///< Высота антенны (в метрах).
    double sourceDepth = 0.15;   ///< Глубина центра (в метрах).
};

#endif // SOURCE_H
