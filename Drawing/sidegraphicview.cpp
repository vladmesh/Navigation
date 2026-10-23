#include "sidegraphicview.h"

SideGraphicView::SideGraphicView(QWidget* parent)
: QGraphicsView(parent), scene(new QGraphicsScene(this))
{
    setScene(scene);
    drawShip();
    drawAxes();
    getSceneProperties();
    drawCenter();
}

SideGraphicView::~SideGraphicView()
{
    for (auto i: VectorGraphicsTextVault){
        delete i;
    }
    for (auto i: VectorAllOtherGraphicsVault){
        delete i;
    }
}

void SideGraphicView::drawLineToTowed(int x1, int z1, int x2, int z2) {
    QGraphicsLineItem* line = new QGraphicsLineItem(x1, z1, x2, z2);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setStyle(Qt::DotLine);
    line->setPen(pen);
    scene->addItem(line);
    VectorAllOtherGraphicsVault.append(line);
}

void SideGraphicView::drawStreamer(int x1, int z1, int x2, int z2, uint n)
{
    QGraphicsLineItem* line = new QGraphicsLineItem(x1, z1, x2, z2);
    QPen pen;
    pen.setColor(Qt::cyan);
    pen.setStyle(Qt::DotLine);
    float dx = (x2-x1)/(n-1);
    float dz = (z2-z1)/(n-1);
    for (float x = x1, z = z1; x <=x2 && z <= z2; x+=dx, z += dz) {
        QGraphicsEllipseItem *pointItem =
            new QGraphicsEllipseItem(x-0.5, z-0.5, 1, 1);
        pointItem->setBrush(QBrush(Qt::magenta));
        scene->addItem(pointItem);
        VectorAllOtherGraphicsVault.append(pointItem);
    }
    line->setPen(pen);
    scene->addItem(line);
    VectorAllOtherGraphicsVault.append(line);
}


void SideGraphicView::addPoint(float x, float y, float z, QString name)
{
    Q_UNUSED(y); //wow
    QGraphicsEllipseItem *pointItem =
        new QGraphicsEllipseItem(x-2.5, z-2.5, 5, 5);
    pointItem->setBrush(QBrush(Qt::red));
    scene->addItem(pointItem);
    QGraphicsTextItem* newItemNameLabel = new QGraphicsTextItem(name);
    newItemNameLabel->setPos(x,z);
    VectorGraphicsTextVault.append(newItemNameLabel);
    VectorAllOtherGraphicsVault.append(pointItem);
    scene->addItem(newItemNameLabel);
}


void SideGraphicView::drawShip() {
    ///get ship configuration
    QPolygonF fig;
    fig << QPointF(-100, 20)
        << QPointF(-100, -10)
        << QPointF(-70, -10)
        << QPointF(-70, -40)
        << QPointF(-30, -40)
        << QPointF(-30, -10)
        << QPointF(110, -10)
        << QPointF(100, 20);
    MyShip = new QGraphicsPolygonItem(fig);
    MyShip->setBrush(QBrush(Qt::blue));
    ///SHIP DRAWING
    scene->addItem(MyShip);
}

void SideGraphicView::drawCenter() {
    QGraphicsEllipseItem *pointItem =
        new QGraphicsEllipseItem(-2.5, -2.5, 5, 5);
    pointItem->setBrush(QBrush(Qt::green));
    scene->addItem(pointItem);

    ///LABEL "SHIP CENTER"
    QGraphicsTextItem* сenterLabel = new QGraphicsTextItem("Ship Center");
    сenterLabel->setPos(10,-20);
    сenterLabel->setDefaultTextColor(Qt::green);
    QFont font = сenterLabel->font();
    font.setBold(true);
    сenterLabel->setFont(font);
    scene->addItem(сenterLabel);
    VectorGraphicsTextVault.append(сenterLabel);
    ///LABEL "SHIP CENTER"
}

void SideGraphicView::drawAxes()
{
    // Ось X
    QGraphicsLineItem* xAxis = new QGraphicsLineItem(-200, 0, 200, 0);
    scene->addItem(xAxis);
    QGraphicsTextItem* xLabel = new QGraphicsTextItem("X");
    xLabel->setPos(190, -30);
    scene->addItem(xLabel);

    // Ось Z
    QGraphicsLineItem* zAxis = new QGraphicsLineItem(0, -200, 0, 200);
    scene->addItem(zAxis);
    QGraphicsTextItem* zLabel = new QGraphicsTextItem("Z");
    zLabel->setPos(20, 190);
    scene->addItem(zLabel);

    VectorGraphicsTextVault.append(xLabel);
    VectorGraphicsTextVault.append(zLabel);

    int Step = 50;
    // Добавление отметок на оси X
    for (int i = -200; i <= 200; i += Step) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(i, -5, i, 5);
        scene->addItem(tick);
        QGraphicsTextItem* label = new QGraphicsTextItem(QString::number(i));
        label->setPos(i - 5, 10);
        scene->addItem(label);
        VectorGraphicsTextVault.append(label);
    }

    // Добавление отметок на оси Y
    for (int i = -200; i <= 200; i += Step) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(-5, i, 5, i);
        scene->addItem(tick);
        VectorAllOtherGraphicsVault.append(tick);
        QGraphicsTextItem* label = new QGraphicsTextItem(QString::number(i));
        label->setPos(-20, i - 5);
        scene->addItem(label);
        VectorGraphicsTextVault.append(label);
    }

    // // Ось Z (не в 3D, но для обозначения)
    // QGraphicsLineItem* zAxis = new QGraphicsLineItem(0, 0, -70, -70);
    // scene->addItem(zAxis);
    // QGraphicsTextItem* zLabel = new QGraphicsTextItem("Z");
    // zLabel->setPos(-80, -80);
    // scene->addItem(zLabel);
}


void SideGraphicView::getSceneProperties() {
    left = scene->sceneRect().left();
    right = scene->sceneRect().right();
    top = scene->sceneRect().top();
    bottom = scene->sceneRect().bottom();
}

void SideGraphicView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zoom out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}


void SideGraphicView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor); // Изменяем курсор при захвате
    }
    // else if (event->button() == Qt::RightButton) { //чисто по приколу
    //     QPointF scenePos = mapToScene(event->pos());
    //     // QMessageBox::information(this, "Coordinates", QString("X: %1, Y: %2").arg(scenePos.x()).arg(scenePos.y()));

    // }
    QGraphicsView::mousePressEvent(event);
}

void SideGraphicView::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning) {
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
    QGraphicsView::mouseMoveEvent(event);
}

void SideGraphicView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor); // Возвращаем курсор обратно
    }
    QGraphicsView::mouseReleaseEvent(event);
}
