#include "lordofgraphic.h"

LordOfGraphic::LordOfGraphic() {
    QGraphicsView view(&scene);
    view.setScene(&scene);
}

LordOfGraphic::~LordOfGraphic()
{

}

void LordOfGraphic::addPoints(FixedItem* item) {
    if (items.size() == 10) {
        delete items.takeFirst(); // Remove and delete the oldest item
    }
    items.append(item);

    QGraphicsEllipseItem* point = new QGraphicsEllipseItem(item->x_coor, item->y_coor, 5, 5);

    point->setBrush(QBrush(Qt::red));
    points.append(point);
    scene.addItem(point);
}

void LordOfGraphic::update()
{

}

LordOfGraphic::itemType LordOfGraphic::getType(FixedItem *item)
{
    QString type = item->metaObject()->className();
    if (type == "Source") {
        return source;
    } else if (type == "Streamer") {
        return streamer;
    } else if (type == "Buoy") {
        return buoy;
    } else if (type == "FixedItem") {
        if (item->x == 0 && item->y == 0) {
            return vessel;
        } else {
            return notNeeded;
        }
    } else {
        return notNeeded;
    }
}
