#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "itemsstorage.h"

#include <QObject>

/**
 * @class Coordinator
 * @brief Класс для управления и расчета координат элементов.
 *
 * Этот класс занимается управлением расчетом координат
 */
class Coordinator : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param parent Родительский объект.
     */
    explicit Coordinator(QObject *parent = nullptr);
    /**
     * @brief Расчет координат элементов.
     *
     * Вызывает расчеты координат для всех элементов, используя данные соединений, если они есть.
     * @return Статус выполнения: 1 - успех, 0 - ошибка.
     */
    int calcCoors();
    /**
     * @brief Вывод координат элементов.
     *
     * Печатает координаты всех элементов в хранилище `Vault`.
     * @return Статус выполнения: 1 - успех, 0 - ошибка.
     */
    int printCoors();
    /**
     * @brief Связывает фиксированные элементы.
     *
     * Устанавливает связи между фиксированными элементами, у которых нет соединений, и одним из элементов с соединением.
     * @return true, если связь установлена, иначе false.
     */
    bool wireFixedItems();
    ItemsStorage* Vault;
    double commonDummyAzimuth; //if end buoy doesn't send az



public slots:
    /**
     * @brief Обработка изменения высоты борта. Старый неиспользуемый код
     *
     * Обновляет высоту для всех элементов, которые зависят от высоты борта.
     * @param boardHeight Новая высота борта.
     */
    void boardDepthChanged(double boardHeight);
};

#endif // COORDINATOR_H
