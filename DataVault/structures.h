#include <QString>

#include "fixeditem.h"
#include "streamer.h"
#include "toweditem.h"
#include "buoy.h"
#include "source.h"

#ifndef STRUCTURES_H
#define STRUCTURES_H

/**
 * @struct SchemeCoordinates
 * @brief Координаты элемента в схеме.
 *
 * Структура хранит 3D-координаты элемента (x, y, z) в схеме.
 */
struct SchemeCoordinates {
    double x; ///< Координата X элемента.
    double y; ///< Координата Y элемента.
    double z; ///< Координата Z элемента.

    /**
     * @brief Конструктор для инициализации координат.
     * @param x Координата X.
     * @param y Координата Y.
     * @param z Координата Z.
     */
    SchemeCoordinates(double x, double y, double z) : x(x), y(y), z(z) {};
};

/**
 * @struct FixedItemInfo
 * @brief Информация о фиксированном элементе на схеме.
 *
 * Структура хранит координаты элемента и его имя.
 */
struct FixedItemInfo {
    SchemeCoordinates SchemeCoors; ///< Координаты элемента.
    QString ItemName; ///< Название элемента.

    /**
     * @brief Конструктор для инициализации информации о фиксированном элементе.
     * @param x Координата X элемента.
     * @param y Координата Y элемента.
     * @param z Координата Z элемента.
     * @param name Название элемента.
     */
    FixedItemInfo(double x, double y, double z, QString name) : SchemeCoors(x, y, z), ItemName(name) {};
};

/**
 * @struct TowedItemInfo
 * @brief Информация о буксируемом элементе.
 *
 * Структура хранит указатель на объект, к которому элемент прикреплён,
 * длину троса, угол наклона и имя элемента.
 */
struct TowedItemInfo {
    FixedItem* toWhoIsWired; ///< Указатель на объект, к которому элемент прикреплён.
    double wireLength; ///< Длина троса.
    double angleToWired; ///< Угол наклона относительно точки крепления.
    QString name; ///< Имя буксируемого элемента.

    /**
     * @brief Конструктор для инициализации информации о буксируемом элементе.
     * @param twiw Указатель на объект, к которому элемент прикреплён.
     * @param wireLength Длина троса.
     * @param angleToWired Угол наклона относительно точки крепления.
     * @param Name Имя буксируемого элемента.
     */
    TowedItemInfo(FixedItem* twiw, double wireLength, double angleToWired, QString Name)
        : toWhoIsWired(twiw), wireLength(wireLength), angleToWired(angleToWired), name(Name) {};
};

/**
 * @struct StreamerInfo
 * @brief Информация о косе.
 *
 * Структура хранит информацию о косе, ее числе каналов
 * и расстоянии между каналами косы.
 */
struct StreamerInfo {
    TowedItemInfo towedInfo; ///< Информация о буксируемом объекте.
    uint numOfChannels; ///< Число каналов стримера.
    QVector<double> chans; ///< Вектор расстояний между каналами стримера.

    /**
     * @brief Конструктор для инициализации информации о стримере.
     * @param towedInfo Информация о буксируемом объекте.
     * @param numOfChannels Число каналов стримера.
     * @param chans Вектор расстояний между каналами стримера.
     */
    StreamerInfo(TowedItemInfo towedInfo, uint numOfChannels, QVector<double> chans)
        : towedInfo(towedInfo), numOfChannels(numOfChannels), chans(chans) {};
};

/**
 * @struct BuoyInfo
 * @brief Информация о буе.
 *
 * Структура хранит информацию о буксируемом объекте, высоту антенны и глубину буксировки буя.
 */
struct BuoyInfo {
    TowedItemInfo towedInfo; ///< Информация о буксируемом объекте.
    double AnthenaHeight; ///< Высота антенны буя.
    double towingDepth; ///< Глубина буксировки буя.

    /**
     * @brief Конструктор для инициализации информации о буе.
     * @param info Информация о буксируемом объекте.
     * @param AnthenaHeight Высота антенны буя.
     * @param towingDepth Глубина буксировки буя.
     */
    BuoyInfo(TowedItemInfo info, double AnthenaHeight, double towingDepth)
        : towedInfo(info), AnthenaHeight(AnthenaHeight), towingDepth(towingDepth) {};
    BuoyInfo(TowedItemInfo info)
        : towedInfo(info), AnthenaHeight(0), towingDepth(0) {};
};

/**
 * @struct SourceInfo
 * @brief Информация о источнике.
 *
 * Структура хранит информацию о буксируемом объекте, который является источником.
 */
struct SourceInfo {
    TowedItemInfo info = {nullptr, -1, -1, ""}; ///< Информация о буксируемом объекте.
    double AnthenaHeight = -1; ///< Высота антенны буя.
    double sourceDepth = -1; ///< Глубина буксировки буя.

    /**
     * @brief Конструктор для инициализации информации о источнике.
     * @param info Информация о буксируемом объекте.
     */
    SourceInfo(TowedItemInfo info, double AnthenaHeight = 1, double sourceDepth = 0.15) : info(info), AnthenaHeight(AnthenaHeight), sourceDepth(sourceDepth) {};
    // SourceInfo(TowedItemInfo info, double AnthenaHeight, double sourceDepth) : info(info), AnthenaHeight(AnthenaHeight), sourceDepth(sourceDepth) {};
};

#endif // STRUCTURES_H
