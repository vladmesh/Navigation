#ifndef UBERPAINTER_H
#define UBERPAINTER_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsView>
#include <QMutex>
#include <QScrollBar>
#include <QThread>
#include <QTimer>
#include <QPointF>
#include <QWheelEvent>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <cmath>
// #include <deque>
#include <QQueue>

#include "itemsstorage.h"

// #include "structures.h"


#define QUEUES_SIZE 5

/**
 * @class UberPainter
 * @brief Класс для визуализации и управления отображением элементов на сцене.
 *
 * Этот класс использует QGraphicsView для создания и управления сценой с элементами,
 * представляющими разные типы объектов
 * Поддерживает функции масштабирования, панорамирования, рисования сетки координат
 * и измерения расстояний между элементами.
 */
class UberPainter : public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор, инициализирует сцену и контейнер для элементов.
     * @param storage Указатель на хранилище элементов.
     * @param parent Указатель на родительский виджет.
     */
    explicit UberPainter(ItemsStorage* storage, QWidget* parent = nullptr);

    /**
     * @brief Деструктор, удаляет созданную сцену.
     */
    ~UberPainter();

    /**
     * @brief Запускает визуализацию, обновляет представление карты.
     */
    void run();

    /**
     * @brief Удаляет элемент из отображения.
     * @param toDelete Указатель на элемент, который нужно удалить.
     */
    void eraseFromPainting(FixedItem* toDelete);

private:
    /**
     * @brief Рисует сетку координат на сцене.
     * @param minX Минимальное значение X.
     * @param minY Минимальное значение Y.
     * @param maxX Максимальное значение X.
     * @param maxY Максимальное значение Y.
     */
    void drawCoordinatesNet(double minX, double minY, double maxX, double maxY);

    void centerView();

    /**
     * @brief Обновляет вид на сцене, очищает и перерисовывает элементы.
     */
    void updateView();

    /**
     * @brief Собирает положения объектов в набор очередей (queue), удаляет слишком старые точки
     */
    void collectItems();

    /**
     * @brief Обновляет отображаемые координаты при панорамировании или масштабировании.
     */
    void updateCoordinates();

    /**
     * @brief Возвращает координаты для элемента типа FixedItem.
     * @param item Указатель на элемент.
     * @return QPointF с координатами элемента.
     */
    QPointF getCoor(FixedItem* item);

    /**
     * @brief Возвращает координаты каналов для элемента типа Streamer.
     * @param streamer Указатель на элемент Streamer.
     * @return QVector<QPointF> с координатами каналов.
     */
    QVector<QPointF> getCoor(Streamer* streamer);

    /**
     * @brief Инициализирует контейнер для хранения позиций элементов.
     */
    void createContainer();

    ItemsStorage* MyVault = nullptr; ///< Указатель на хранилище элементов.
    QGraphicsScene* graphicsScene = nullptr; ///< Сцена для визуализации элементов.
    QMutex sceneMutex; ///< Мьютекс для защиты доступа к сцене.

    QMap<FixedItem*, QQueue<QPointF>> recentPositions; ///< набор пар объект (ключ) и последних позиций(значение).

    /**
     * @brief Обрабатывает событие масштабирования колеса мыши.
     * @param event Указатель на событие колеса мыши.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Обрабатывает событие нажатия кнопки мыши.
     * @param event Указатель на событие нажатия кнопки мыши.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Обрабатывает событие движения мыши.
     * @param event Указатель на событие движения мыши.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Обрабатывает событие отпускания кнопки мыши.
     * @param event Указатель на событие отпускания кнопки мыши.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool isPanning = false; ///< Флаг, указывающий на режим панорамирования.
    QPoint lastMousePos; ///< Последняя позиция мыши.
    QPointF startPoint; ///< Начальная точка для измерения расстояния.
    QGraphicsLineItem* distanceLine = new QGraphicsLineItem(); ///< Линия для измерения расстояния.
    QGraphicsTextItem* distanceLineLen = new QGraphicsTextItem(); ///< Текстовое отображение длины линии.
    QGraphicsTextItem* distanceLineAz = new QGraphicsTextItem(); ///< Текстовое отображение азимута линии.

    double minX; ///< Минимальное значение X на сцене.
    double minY; ///< Минимальное значение Y на сцене.
    double maxX; ///< Максимальное значение X на сцене.
    double maxY; ///< Максимальное значение Y на сцене.

    bool isNotDrawingLine = true; ///< Флаг, указывающий на режим рисования линии.

};
#endif // UBERPAINTER_H
