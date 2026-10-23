#ifndef VIRTUALOBSERVER_H
#define VIRTUALOBSERVER_H


#include <QObject>

#include "structures.h"
/**
 * @class VirtualObserver
 * @brief Абстрактный базовый класс для наблюдателей, реализующих паттерн "наблюдатель".
 *
 * Класс определяет интерфейс для наблюдения за изменениями статуса. Производные классы должны
 * реализовать метод `react()` для обработки событий.
 */
class VirtualObserver : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса VirtualObserver.
     */
    VirtualObserver();

    /**
     * @brief Виртуальный деструктор класса VirtualObserver.
     */
    virtual ~VirtualObserver();

    /**
     * @brief Чисто виртуальный метод для реализации реакции на событие.
     *
     * Производные классы должны определить поведение в этом методе.
     */
    virtual void react() = 0;

private slots:
    /**
     * @brief Чисто виртуальный слот для обработки изменения статуса.
     */
    virtual void statusChanged() = 0;
};

#endif // VIRTUALOBSERVER_H
