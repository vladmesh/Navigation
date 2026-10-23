#ifndef OBJECTOBSERVER_H
#define OBJECTOBSERVER_H

#include <QObject>
#include <QTableWidget>

#include "itemsstorage.h"
#include "virtualobserver.h"
/**
 * @class ObjectObserver
 * @brief Класс для наблюдения за объектами и их статусом соединения.
 * Также занимается обновлением статуса в таблмчке с объектами в главном окне программы
 *
 * Класс `ObjectObserver` отслеживает изменения в объектах, выводя их статус
 * соединения в таблицу. Реализует интерфейс `VirtualObserver`.
 */
class ObjectObserver : public VirtualObserver
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор по умолчанию.
     */
    ObjectObserver();

    /**
     * @brief Конструктор с инициализацией хранилища объектов.
     * @param Vault Указатель на `ItemsStorage`, содержащий объекты `FixedItem`.
     */
    ObjectObserver(ItemsStorage* Vault);

    /**
     * @brief Деструктор класса ObjectObserver.
     */
    ~ObjectObserver() override;

    /**
     * @brief Реализация метода реакции на изменение статуса.
     * Этот метод обновляет элементы таблицы `tableWithItems` на основе текущего статуса объектов.
     */
    void react() override;

    /**
     * @brief Устанавливает таблицу, в которой отображаются статусы объектов.
     * @param newTableWithItems Указатель на `QTableWidget` для отображения статусов объектов.
     */
    void setTableWithItems(QTableWidget *newTableWithItems);
    /**
     * @brief Указатель на хранилище элементов `ItemsStorage`.
     */
    ItemsStorage* Vault;

    /**
     * @brief Указатель на `QTableWidget` для отображения статуса элементов.
     */
    QTableWidget* tableWithItems;
    QPointer<QTimer> checkTimer;
public slots:
    /**
     * @brief Обрабатывает изменения статуса.
     * Этот слот вызывает метод `react()`.
     * пока не используется
     */
    void statusChanged() override;

    void timeToCheckState();

    /**
     * @brief Обновляет статус соединения в таблице при изменении соединения объекта `FixedItem`.
     * @param who Указатель на `FixedItem`, у которого изменился статус соединения.
     */
    void hasConnectionChanged(FixedItem *who);

    /**
     * @brief Добавляет объект в наблюдаемые и подключает сигналы.
     * @param who Указатель на объект `FixedItem`, который добавляется в наблюдаемые.
     */
    void itemAdded(FixedItem* who);


};

#endif // OBJECTOBSERVER_H
