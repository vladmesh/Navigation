#ifndef LORDOFGRAPHIC_H
#define LORDOFGRAPHIC_H

#include <QObject>
#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include "itemsstorage.h"
// #include "logger.h"

class LordOfGraphic : public QGraphicsView
{
    Q_OBJECT
public:
    LordOfGraphic();
    ~LordOfGraphic();
    void addPoints(FixedItem* item);

public slots:
    void update();

private:
    enum itemType{
        source,
        buoy,
        streamer,
        vessel,
        notNeeded
    };

    int numberOfPoints;
    QVector<FixedItem*> items;
    QVector<QGraphicsEllipseItem*> points;
    QGraphicsScene scene;
    void updateTransparency();
    LordOfGraphic::itemType getType(FixedItem* item);


};

#endif // LORDOFGRAPHIC_H
