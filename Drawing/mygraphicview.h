#ifndef MYGRAPHICVIEW_CPP
#define MYGRAPHICVIEW_CPP

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QPolygonF>
#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QWheelEvent>
#include <QScrollBar>
#include <QString>
#include <QMessageBox>

#include "itemsstorage.h"
#include "hoverpaintingrpoint.h"

/**
 * @class MyGraphicView
 * @brief Класс для отображения и управления графическим представлением сцены.
 *
 * Класс позволяет отображать и настраивать элементы сцены, такие как корабль, координатные оси,
 * сетка и добавленные точки. Также поддерживает масштабирование, панорамирование и отрисовку
 * линий к буксируемым объектам.
 */
class MyGraphicView : public QGraphicsView
{
public:


    /**
     * @brief Конструктор, инициализирует сцену и элементы интерфейса.
     * @param parent Указатель на родительский виджет.
     */
    MyGraphicView(QWidget* parent = nullptr);

    /**
     * @brief Деструктор, удаляет элементы сцены.
     */
    ~MyGraphicView();

    /**
     * @brief Добавляет точку на сцену с указанными координатами.
     * @param x Координата X.
     * @param y Координата Y.
     * @param z Координата Z.
     * @param name Название точки.
     */
    void addPoint(float x, float y, float z, QString name);

    // Инициализация отрисовки
    /**
     * @brief Отрисовывает схематическое изображение корабля.
     */
    void drawShip();

    /**
     * @brief Отмечает центр корабля на сцене.
     */
    void drawCenter();

    /**
     * @brief Отрисовывает координатные оси.
     */
    void drawAxes();

    /**
     * @brief Отрисовывает сетку на сцене.
     */
    void drawGrid();

    /**
     * @brief Получает свойства сцены (границы).
     */
    void getSceneProperties();
    // Инициализация отрисовки

    // Свойства отрисовки
    int X_center = 0; ///< Центр по оси X.
    int Y_center = 0; ///< Центр по оси Y.
    float left; ///< Левая граница сцены.
    float right; ///< Правая граница сцены.
    float top; ///< Верхняя граница сцены.
    float bottom; ///< Нижняя граница сцены.
    // Свойства отрисовки

    /**
     * @brief Отрисовывает линию к буксируемому объекту.
     * @param x1 Начальная координата X.
     * @param y1 Начальная координата Y.
     * @param x2 Конечная координата X.
     * @param y2 Конечная координата Y.
     */
    void drawLineToTowed(int x1, int y1, int x2, int y2);

    /**
     * @brief Отрисовывает линию стримера с равномерно распределенными точками.
     * @param x1 Начальная координата X.
     * @param y1 Начальная координата Y.
     * @param x2 Конечная координата X.
     * @param y2 Конечная координата Y.
     * @param n Количество точек на линии.
     */
    void drawStreamer(int x1, int y1, int x2, int y2, uint n);
    void drawStreamer(QPointer<Streamer> str);


    void update();
    void updateShip();
    void setVault(ItemsStorage* newVault);


    QGraphicsPolygonItem* MyShip = nullptr; ///< Объект для представления корабля.
    QGraphicsScene* scene = nullptr; ///< Сцена для отображения графических элементов.
    void setShipFig(const QPolygonF &newShipFig);

private:
    /**
     * @brief Обрабатывает событие двойного нажатия кнопки мыши.
     * нужно для добавления вершин
     * @param event Указатель на событие нажатия кнопки мыши.
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

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
    QPoint lastMousePos; ///< Последняя позиция мыши при панорамировании.

    bool isMovingVertex = false; ///< буль если мы двигаем
    QGraphicsTextItem *vertexTextItem = nullptr;
    int movingVertexIndex = -1;
    QPolygonF shipFig;

    QVector<QGraphicsTextItem*> VectorGraphicsTextVault; ///< Вектор для хранения текстовых элементов.
    QVector<QGraphicsItem*> VectorAllOtherGraphicsVault; ///< Вектор для хранения графических элементов.


    ItemsStorage* Vault = nullptr;


    void saveVerticesToFile(const QString &fileName);
    void resetToDefault();
    void loadVerticesFromFile(const QString &fileName);
    const QString shipFileName = "./shipEdges.txt";
};

#endif // MYGRAPHICVIEW_CPP
