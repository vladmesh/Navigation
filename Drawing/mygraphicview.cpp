#include "mygraphicview.h"


MyGraphicView::MyGraphicView(QWidget* parent)
: QGraphicsView(parent), scene(new QGraphicsScene(this))
{
    setScene(scene);
    QGraphicsView view(scene);
    QTransform transform;
    transform.scale(1, -1);  // Ось Y теперь направлена вниз
    view.setTransform(transform);

    loadVerticesFromFile(shipFileName);
    drawAxes();
    getSceneProperties();
    view.show();
    scene->setSceneRect(scene->sceneRect().left()-200, scene->sceneRect().top()-200, 600, 600);

}

MyGraphicView::~MyGraphicView(){
    saveVerticesToFile(shipFileName);
}


void MyGraphicView::addPoint(float x, float y, float z, QString name)
{
    Q_UNUSED(z); //wow
    HoverPaintingPoint *pointItem = new HoverPaintingPoint(x, y, name);
    scene->addItem(pointItem);


}

void MyGraphicView::drawShip() {
    if (MyShip) {
        MyShip = new QGraphicsPolygonItem(shipFig);
        MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
        scene->addItem(MyShip);
    } else {
        shipFig<< QPointF(-50, -20)
            << QPointF(-50, 20)
            << QPointF(55, 20)
            << QPointF(75, 0)
            << QPointF(55, -20);
        MyShip = new QGraphicsPolygonItem(shipFig);
        MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
        scene->addItem(MyShip);
    }
    ///SHIP DRAWING
}

void MyGraphicView::drawCenter() {
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

void MyGraphicView::drawAxes()
{
    // Ось X

    QGraphicsLineItem* xAxis = new QGraphicsLineItem(-100, -100, 100, -100); //(-100, 0, 100, 0);
    scene->addItem(xAxis);
    QGraphicsTextItem* xLabel = new QGraphicsTextItem("X");
    xLabel->setPos(95-2, -130+5);
    scene->addItem(xLabel);

    // Ось Y
    QGraphicsLineItem* yAxis = new QGraphicsLineItem(100, -100, 100, 100); // (0, -100, 0, 100)
    scene->addItem(yAxis);
    QGraphicsTextItem* yLabel = new QGraphicsTextItem("Y");
    yLabel->setPos(95-2, 100);
    scene->addItem(yLabel);

    VectorGraphicsTextVault.append(xLabel);
    VectorGraphicsTextVault.append(yLabel);


    QFont font;
    font.setPointSize(8);
    int Step = 25;

    // Добавление отметок на оси X
    for (int i = -100; i < 100; i += Step) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(i, -105, i, -95);
        scene->addItem(tick);
        QGraphicsTextItem* label = new QGraphicsTextItem(QString::number(i));

        i<0?label->setPos(i-15, -125):label->setPos(i-5, -125);

        label->setFont(font);
        scene->addItem(label);
        VectorGraphicsTextVault.append(label);
    }

    // Добавление отметок на оси Y
    for (int i = -100; i <= 100; i += Step) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(95, i, 105, i);
        scene->addItem(tick);
        VectorAllOtherGraphicsVault.append(tick);
        QGraphicsTextItem* label = new QGraphicsTextItem(QString::number(i));
        label->setPos(110, i-15);
        label->setFont(font);
        scene->addItem(label);
        VectorGraphicsTextVault.append(label);
    }
}


void MyGraphicView::drawGrid(){ ///TODO:сделать нормально
    // Horizontal lines
    for (int i = bottom; i <= top; i += (top-bottom)/20)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(left, i, right, i);
        scene->addItem(line);
        VectorAllOtherGraphicsVault.append(line);
    }

    // Vertical lines
    for (int i = left; i <= right; i += (right-left)/20)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(i, bottom, i, top);
        scene->addItem(line);
        VectorAllOtherGraphicsVault.append(line);
    }
}

void MyGraphicView::getSceneProperties() {
    left = scene->sceneRect().left();
    right = scene->sceneRect().right();
    top = scene->sceneRect().top();
    bottom = scene->sceneRect().bottom();
}

void MyGraphicView::drawLineToTowed(int x1, int y1, int x2, int y2) {
    QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setStyle(Qt::DotLine);
    line->setPen(pen);
    scene->addItem(line);
    VectorAllOtherGraphicsVault.append(line);
}

void MyGraphicView::drawStreamer(int x1, int y1, int x2, int y2, uint n)
{
    QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
    QPen pen;
    pen.setColor(Qt::cyan);
    pen.setStyle(Qt::DotLine);
    float dx = (x2-x1)/(n-1);
    float dy = (y2-y1)/(n-1);
    for (float x = x1, y = y1; x <=x2 && y <= y2; x+=dx, y += dy) {
        QGraphicsEllipseItem *pointItem =
            new QGraphicsEllipseItem(x-0.5, y-0.5, 1, 1);
        pointItem->setBrush(QBrush(Qt::magenta));
        scene->addItem(pointItem);
        VectorAllOtherGraphicsVault.append(pointItem);
    }
    line->setPen(pen);
    scene->addItem(line);
    VectorAllOtherGraphicsVault.append(line);
}

void MyGraphicView::drawStreamer(QPointer<Streamer> str)
{

    QGraphicsEllipseItem *pointItem =
        new QGraphicsEllipseItem(str->x-0.5, str->y-0.5, 1, 1);
    pointItem->setBrush(QBrush(Qt::magenta));
    scene->addItem(pointItem);
    VectorAllOtherGraphicsVault.append(pointItem);
    for (uint i = 1; i < str->getChanCount(); i++) {
        pointItem = new QGraphicsEllipseItem(str->x-0.5 - str->chans[i], str->y-0.5, 1, 1);
        pointItem->setBrush(QBrush(Qt::magenta));
        scene->addItem(pointItem);
        VectorAllOtherGraphicsVault.append(pointItem);
    }
}

void MyGraphicView::update()
{
    scene->clear();
    drawShip();
    drawAxes();
    for (auto i: Vault->ItemsVault) {
        if (i->itemType == "Fixed") {
            addPoint(i->x, i->y, 0, i->name);
        } else if (i->itemType == "Streamer") {
            drawStreamer(qobject_cast<Streamer*>(i));
        } else {
            TowedItem* towed = qobject_cast<TowedItem*>(i);
            addPoint(i->x, i->y, 0, i->name);
            drawLineToTowed(towed->x, towed->y,towed->towingPoint->x, towed->towingPoint->y);
        }

    }
}

void MyGraphicView::updateShip()
{
    delete MyShip;
    MyShip = new QGraphicsPolygonItem(shipFig);
    MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
    scene->addItem(MyShip);
}


void MyGraphicView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zoom out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void MyGraphicView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    shipFig = MyShip->polygon();

    // Проверяем, попадает ли клик в существующую вершину
    bool vertexFound = false;
    for (int i = 0; i < shipFig.size(); ++i) {
        if (QLineF(scenePos, shipFig[i]).length() < 10) { // Точность в 10 пикселей
            // Удаляем найденную вершину
            shipFig.removeAt(i);
            vertexFound = true;
            break;
        }
    }

    if (!vertexFound) {
        // Если клик не попал в вершину, добавляем новую
        shipFig<< scenePos;
    }

    MyShip->setPolygon(shipFig);
    MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
    QGraphicsView::mouseDoubleClickEvent(event);
}

void MyGraphicView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor); // Изменяем курсор при захвате
    } else if (event->button() == Qt::RightButton) {
        QPointF scenePos = mapToScene(event->pos());
        shipFig = MyShip->polygon();

        // Проверяем, близко ли точка клика к какой-либо вершине
        for (int i = 0; i < shipFig.size(); ++i) {
            if (QLineF(scenePos, shipFig[i]).length() < 10) { // Точность в 10 пикселей
                isMovingVertex = true;
                movingVertexIndex = i; // Сохраняем индекс вершины
                break;
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicView::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning) {
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    } else if (isMovingVertex) {
        QPointF scenePos = mapToScene(event->pos());
        shipFig = MyShip->polygon();

        // Обновляем координаты выбранной вершины
        shipFig[movingVertexIndex] = scenePos;
        MyShip->setPolygon(shipFig);
        MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));

        // Если текстовый элемент ещё не создан, создаём его
        if (!vertexTextItem) {
            vertexTextItem = new QGraphicsTextItem();
            vertexTextItem->setDefaultTextColor(Qt::red);
            scene->addItem(vertexTextItem);
        }

        // Обновляем текст и позицию
        vertexTextItem->setPlainText(QString("X: %1, Y: %2").arg((int)scenePos.x()).arg((int)scenePos.y()));
        vertexTextItem->setPos(scenePos + QPointF(10, -20)); // Смещение текста относительно вершины
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MyGraphicView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor); // Возвращаем курсор обратно
    } else if (event->button() == Qt::RightButton && isMovingVertex) {
        isMovingVertex = false; // Завершаем перемещение вершины

        // Удаляем текстовый элемент после завершения перемещения
        if (vertexTextItem) {
            scene->removeItem(vertexTextItem);
            delete vertexTextItem;
            vertexTextItem = nullptr;
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MyGraphicView::setShipFig(const QPolygonF &newShipFig)
{
    shipFig = newShipFig;
}

void MyGraphicView::setVault(ItemsStorage* newVault)
{
    Vault = newVault;
}

void MyGraphicView::resetToDefault()
{
    shipFig.clear();
    shipFig<< QPointF(-50, -20)
        << QPointF(-50, 20)
        << QPointF(55, 20)
        << QPointF(75, 0)
        << QPointF(55, -20);
    MyShip = new QGraphicsPolygonItem(shipFig);
    MyShip->setPolygon(shipFig);
    MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
    scene->addItem(MyShip);
}

void MyGraphicView::loadVerticesFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        int count = 0;
        shipFig.clear();
        while (!in.atEnd()) {
            qreal x, y;
            in >> x >> y;
            if (x == 0 && y == 0) continue;
            shipFig << QPointF(x, y);
            count++;
        }
        file.close();
        if (count < 3) {
            resetToDefault();
            return;
        } else {
            MyShip = new QGraphicsPolygonItem(shipFig);
            MyShip->setPolygon(shipFig);
            MyShip->setBrush(QBrush(QColorConstants::Svg::lightblue));
            scene->addItem(MyShip);
        }
    } else {
        resetToDefault();
    }
}

void MyGraphicView::saveVerticesToFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        shipFig = MyShip->polygon();
        QTextStream out(&file);
        for (const QPointF &point : shipFig) {
            if (point.x() == 0 && point.y() == 0) continue;
            out << (int)point.x() << " " << (int)point.y() << "\n";
        }
        file.close();
    }
}
