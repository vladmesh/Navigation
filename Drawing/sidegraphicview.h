#ifndef SIDEGRAPHICVIEW_H
#define SIDEGRAPHICVIEW_H

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

#include "mygraphicview.h"

class SideGraphicView : public QGraphicsView
{
    Q_OBJECT
public:
    SideGraphicView(QWidget* parent = nullptr);
    ~SideGraphicView();


    ///add point to side view
    void addPoint(float x, float y, float z, QString name);

    ///Draw line for towed Item
    void drawLineToTowed(int x1, int z1, int x2, int z2);
    void drawStreamer(int x1, int y1, int x2, int y2, uint n);

private:
    ///init drawing
    void drawShip();
    void drawCenter();
    void drawAxes();
    void drawGrid();
    void getSceneProperties();
    ///init drawing

    ///Drawing properties
    int X_center = 0;
    int Y_center = 0;
    float left;
    float right;
    float top;
    float bottom;
    ///Drawing properties

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool isPanning = false;
    QPoint lastMousePos;


    QGraphicsScene* scene = nullptr;
    QGraphicsPolygonItem* MyShip = nullptr;
    QVector<QGraphicsTextItem*> VectorGraphicsTextVault;
    QVector<QGraphicsItem*> VectorAllOtherGraphicsVault;
};

#endif // SIDEGRAPHICVIEW_H
