#ifndef HOVERPAINTINGRPOINT_H
#define HOVERPAINTINGRPOINT_H

#include <QGraphicsEllipseItem >
#include <QBrush>
#include <QGraphicsSceneHoverEvent>

class HoverPaintingPoint : public QGraphicsEllipseItem {
public:
    HoverPaintingPoint(float x, float y, QString name, QGraphicsItem *parent = nullptr)
        : QGraphicsEllipseItem(x - 2.5, y - 2.5, 5, 5, parent), pointName(name) {
        setBrush(QBrush(Qt::red));
        setAcceptHoverEvents(true); // Включаем обработку наведения мыши
        label = new QGraphicsTextItem(name, this); // Текст будет дочерним элементом
        label->setVisible(false); // Прячем текст по умолчанию
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override {
        label->setPos(event->pos().x() + 10, event->pos().y() - 10); // Смещение текста относительно курсора
        label->setVisible(true); // Показываем текст
        QGraphicsEllipseItem::hoverEnterEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override {
        label->setVisible(false); // Скрываем текст
        QGraphicsEllipseItem::hoverLeaveEvent(event);
    }

private:
    QString pointName;               // Имя точки
    QGraphicsTextItem *label;        // Элемент для отображения текста
};

#endif // HOVERPAINTINGRPOINT_H
