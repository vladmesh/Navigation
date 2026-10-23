#ifndef FABRIC_H
#define FABRIC_H

#include <QString>
#include <QObject>

#include "fixeditem.h"
#include "toweditem.h"
#include "streamer.h"
#include "source.h"
#include "connection_creator.h"
#include "streamerdialog.h"
#include "buoydialog.h"
#include "sourcedialog.h"
#include "structures.h"

/**
 * @class Fabric
 * @brief Класс, отвечающий за создание и настройку объектов `FixedItem` и наследников и их подключение к соединениям.
 *
 * `Fabric` используется для создания элементов, таких как `FixedItem`, `TowedItem`, `Streamer`, `Buoy` и `Source`,
 * а также для управления их подключением к соответствующим объектам соединения - наследников `Connection`.
 */
class Fabric : public QObject
{
// Q_OBJECT

public:
    /**
     * @brief Конструктор класса `Fabric`.
     */
    Fabric();
    ~Fabric();
    /**
     * @brief Создает объект типа `FixedItem`.
     * @param info Структура с информацией о `FixedItem`.
     * @param needConnect Указывает, требуется ли подключение.
     * @return Указатель на созданный объект `FixedItem`.
     */
    FixedItem* CreateItem(FixedItemInfo info, bool needConnect);
    /**
     * @brief Создает объект типа `TowedItem`.
     * @param info Структура с информацией о `TowedItem`.
     * @param needConnect Указывает, требуется ли подключение.
     * @return Указатель на созданный объект `TowedItem`.
     */
    TowedItem* CreateItem(TowedItemInfo info, bool needConnect);
    /**
     * @brief Создает объект типа `Streamer`.
     * @param info Структура с информацией о `Streamer`.
     * @return Указатель на созданный объект `Streamer`.
     */
    Streamer* CreateItem(StreamerInfo info);
    /**
     * @brief Создает объект типа `Buoy`.
     * @param info Структура с информацией о `Buoy`.
     * @param needConnection Указывает, требуется ли подключение.
     * @return Указатель на созданный объект `Buoy`.
     */
    Buoy* CreateItem(BuoyInfo info, bool needConnection);
    /**
     * @brief Создает объект типа `Source`.
     * @param info Структура с информацией о `Source`.
     * @param needConnect Указывает, требуется ли подключение.
     * @return Указатель на созданный объект `Source`.
     */
    Source* CreateItem(SourceInfo info, bool needConnect);

    /**
     * @brief Создает соединение для указанного элемента.
     * @param itemName Имя элемента, для которого создается соединение.
     * @return Указатель на созданный объект `Connection`.
     */
    Connection* createConnection(QString itemName);

    /**
     * @brief Привязывает соединение к объекту .
     * @param to Указатель на объект , к которому привязывается соединение.
     * @param who Указатель на объект наследник `Connection`, который привязывается к элементу.
     * @return флаг, указывающий на успешное или неудачное завершение.
     */
    int bindItemConnection(FixedItem* to, Connection* who);

    /**
     * @brief Устанавливает хранилище элементов для текущего объекта `Fabric`.
     * @param MyVault Указатель на объект `ItemsStorage`.
     */
    void setMyVault(ItemsStorage* MyVault);

    Connection_creator* connectionCreator = nullptr;///< Указатель на создатель соединений `Connection_creator`.
private:
    ItemsStorage* MyVault = nullptr;///< Указатель на хранилище объектов `ItemsStorage`.
    bool hasItemForBuffer = false; ///< Флаг, указывающий на наличие элемента для буфера.
private slots:
    /**
     * @brief Слот, вызываемый при создании соединения.
     */
    void connectionCreated();
};

#endif // FABRIC_H
