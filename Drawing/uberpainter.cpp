#include "uberpainter.h"


UberPainter::UberPainter(ItemsStorage *storage, QWidget *parent) :
    QGraphicsView(parent), MyVault(storage), graphicsScene(new QGraphicsScene)
{
    setScene(graphicsScene);
    this->resetTransform();         // Сброс трансформаций
    this->scale(1, -1);
    createContainer();
    setDragMode(QGraphicsView::ScrollHandDrag);
}

UberPainter::~UberPainter() ///some comments
{
    delete graphicsScene;
}

void UberPainter::run() //срабатывает по таймеру
{//reading item's position and building map
    collectItems();
    if (isNotDrawingLine) updateView();
    //isDrawingLine - флаг что мы рисуем линии. он снят, пока меряем расстояние от точки до точки на карте
}

void UberPainter::updateCoordinates()
{
    // Удаляем все текстовые элементы, оставляя сетку
    for (auto item : graphicsScene->items()) {
        if (dynamic_cast<QGraphicsTextItem*>(item)) {
            graphicsScene->removeItem(item);
            delete item;
        }
    }

    // Получаем видимую область с учетом инверсии

    // Настраиваем шрифт с динамическим размером
    QFont font;
    font.setPointSizeF(10.0 / transform().m11()); // Привязываем размер шрифта к масштабу


    //отрисовка координат при движении зажатой мыши
    QTransform mirrorTransform;
    mirrorTransform.scale(1, -1);  // Отражаем по вертикали

    QRectF visibleArea = mapToScene(viewport()->geometry()).boundingRect();
    // Рисуем координаты вдоль верхней границы (для X)
    for (double x = visibleArea.left()+(visibleArea.right() - visibleArea.left())/8; x <= visibleArea.right(); x += (visibleArea.right() - visibleArea.left())/8) {
        QGraphicsTextItem* textX = graphicsScene->addText(QString::number(x), font);
        textX->setPos(x, visibleArea.bottom() - 5); // Позиция вдоль инвертированной верхней границы
        textX->setDefaultTextColor(Qt::black);
        textX->setTransform(mirrorTransform);
    }

    // Рисуем координаты вдоль левой границы (для Y)
    for (double y = visibleArea.top() + abs(visibleArea.top() - visibleArea.bottom())/8; y <= visibleArea.bottom() - abs(visibleArea.top() - visibleArea.bottom())/8; y += abs(visibleArea.top() - visibleArea.bottom())/8) {
        QGraphicsTextItem* textY = graphicsScene->addText(QString::number(y,'g', 8), font);
        textY->setPos(visibleArea.left() + 5, y); // Позиция слева
        textY->setDefaultTextColor(Qt::black);
        textY->setTransform(mirrorTransform);
    }
}

void UberPainter::drawCoordinatesNet(double minX, double minY, double maxX, double maxY)
{
    minX -= 50;
    minY -= 50;
    maxX -= 50;
    maxX -= 50;
    const double stepSize = 50.0;
    if (minX == std::numeric_limits<double>::max()) return;
    minX = static_cast<int>(std::round(minX / 50) * 50);
    minY = static_cast<int>(std::round(minY / 50) * 50);
    maxX = static_cast<int>(std::round(maxX / 50) * 50);
    maxY = static_cast<int>(std::round(maxY / 50) * 50);

    QFont smallFont;
    smallFont.setPointSize(6);
    for (double x = minX - 500; x <= maxX + 500; x += stepSize) {
        QGraphicsLineItem* line = graphicsScene->addLine(x, minY - 500, x, maxY + 500, QPen(Qt::lightGray));
        Q_UNUSED(line);
        if (x == minX - 50) continue;


    }

    for (double y = minY - 500; y <= maxY + 500; y += stepSize) {
        QGraphicsLineItem* line = graphicsScene->addLine(minX - 500, y, maxX + 500, y, QPen(Qt::lightGray));
        Q_UNUSED(line);


    }


    QTransform mirrorTransform;
    mirrorTransform.scale(1, -1);
    QFont font;
    font.setPointSizeF(10.0 / transform().m11()); // Привязываем размер шрифта к масштабу
    QRectF visibleArea = mapToScene(viewport()->geometry()).boundingRect();
    // Рисуем координаты вдоль верхней границы (для X)
    for (double x = visibleArea.left()+(visibleArea.right() - visibleArea.left())/8; x <= visibleArea.right(); x += (visibleArea.right() - visibleArea.left())/8) {
        QGraphicsTextItem* textX = graphicsScene->addText(QString::number(x), font);
        textX->setPos(x, visibleArea.bottom() - 5); // Позиция вдоль инвертированной верхней границы
        textX->setDefaultTextColor(Qt::black);
        textX->setTransform(mirrorTransform);
    }

    // Рисуем координаты вдоль левой границы (для Y)
    for (double y = visibleArea.top() + abs(visibleArea.top() - visibleArea.bottom())/8; y <= visibleArea.bottom() - abs(visibleArea.top() - visibleArea.bottom())/8; y += abs(visibleArea.top() - visibleArea.bottom())/8) {
        QGraphicsTextItem* textY = graphicsScene->addText(QString::number(y,'g', 8), font);
        textY->setPos(visibleArea.left() + 5, y); // Позиция слева
        textY->setDefaultTextColor(Qt::black);
        textY->setTransform(mirrorTransform);
    }
}

void UberPainter::centerView()
{
    if (isPanning) return;
    QRectF visibleArea = mapToScene(viewport()->geometry()).boundingRect();
    if (!MyVault->ItemsVault.isEmpty()) {
        QPointF center = QPointF((visibleArea.center().x() + getCoor(MyVault->ItemsVault.first()).x()) / 2.0,
                                 (visibleArea.center().y() + getCoor(MyVault->ItemsVault.first()).y()) / 2.0);

        centerOn(center);
    }


}


void UberPainter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::AllButtons) {
        qDebug() <<"BOTH BUTTONS" <<__FUNCTION__;
        return;
    }
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor); // Изменяем курсор при захвате
    } else if (event->button() == Qt::RightButton) {
        isPanning = true;
        isNotDrawingLine = false;
        startPoint = mapToScene(event->pos());
        setCursor(Qt::BlankCursor); ///setCursor(Qt::BlankCursor);

        distanceLine = new QGraphicsLineItem();
        distanceLineLen = new QGraphicsTextItem();
        distanceLineAz = new QGraphicsTextItem();


    }
    QGraphicsView::mousePressEvent(event);
}

void UberPainter::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning && (event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)) {
        return;
    } else if (isPanning && (event->buttons() & Qt::LeftButton)) { // Проверяем удержание левой кнопки
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        updateCoordinates(); // Обновляем координаты при сдвиге
    } else if (isPanning && (event->buttons() & Qt::RightButton)) {
        isNotDrawingLine = false;
        QPointF cur = mapToScene(event->pos());
        QPointF delta = cur - startPoint;

        double distance = pow(pow(delta.x(),2) + pow(delta.y(),2), 0.5);
        double azimuthDegrees = (qAtan2(delta.x(), delta.y())) * 180 / M_PI;
        if (azimuthDegrees < 0) azimuthDegrees+=360;

        QFont font;
        font.setPointSize(6);    // Небольшой размер
        font.setBold(true);       // Жирное начертание
        QTransform mirrorTransform;
        mirrorTransform.scale(1, -1);

        if (distanceLineLen!= nullptr) delete distanceLineLen;
        if (distanceLineAz!= nullptr) delete distanceLineAz;

        distanceLineLen = graphicsScene->addText(QString::number(distance, 'g', 3) + "m.",font);
        distanceLineAz = graphicsScene->addText(QString::number((int)azimuthDegrees) + "°", font);

        distanceLineLen->setDefaultTextColor(QColor(165,42,42));
        distanceLineLen->setPos(cur.x(), cur.y()+10);

        distanceLineAz->setDefaultTextColor(QColor(165,42,42));
        distanceLineAz->setPos(cur.x(), cur.y()-10);

        distanceLineLen->setTransform(mirrorTransform);
        distanceLineAz->setTransform(mirrorTransform);


        if (distanceLine != nullptr) delete distanceLine;
        distanceLine = graphicsScene->addLine(startPoint.x(),
            startPoint.y(), mapToScene(event->pos()).x(), mapToScene(event->pos()).y(), QPen(QColor(165, 42, 42))); //mr brown
    }

    QGraphicsView::mouseMoveEvent(event);
}

void UberPainter::mouseReleaseEvent(QMouseEvent *event)
{


    if (event->button() == Qt::LeftButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor); // Возвращаем курсор обратно
    }
    if (event->button() == Qt::RightButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor); // Возвращаем курсор обратно
        if (distanceLine != nullptr && !(event->buttons() & Qt::RightButton)) delete distanceLine;
        distanceLine = new QGraphicsLineItem();
        distanceLineLen = new QGraphicsTextItem();
        distanceLineAz = new QGraphicsTextItem();
    }
    QGraphicsView::mouseReleaseEvent(event);
    isNotDrawingLine = true;
    run();//чтобы убрать задержку
}


// Функция для обработки масштабирования.
void UberPainter::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15; // Коэффициент масштабирования

    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor); // Увеличиваем масштаб
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Уменьшаем масштаб
    }

    QGraphicsView::wheelEvent(event);
}

void UberPainter::updateView()
{
    centerView();
    QMutexLocker locker(&sceneMutex);
    graphicsScene->clear();

    minX = std::numeric_limits<double>::max();
    minY = std::numeric_limits<double>::max();
    maxX = std::numeric_limits<double>::lowest();
    maxY = std::numeric_limits<double>::lowest();

    for (auto i: recentPositions) {
        for (QPointF coor: i) {
            if (coor.x() < 10 || coor.y() < 10) continue;
            minX = std::min(minX, coor.x());
            minY = std::min(minY, coor.y());
            maxX = std::max(maxX, coor.x());
            maxY = std::max(maxY, coor.y());
        }
    }


    graphicsScene->setSceneRect(minX - 500, minY - 500, (maxX - minX) + 1000, (maxY - minY) + 1000);


    drawCoordinatesNet(minX, minY, maxX, maxY);

    for (auto it = recentPositions.begin(); it != recentPositions.end(); ++it) {
        FixedItem* item = it.key();
        if (item == nullptr) continue;
        if (item->itemType.isEmpty()) continue; //призраки удаленных объектов
        const QQueue<QPointF>& queue = it.value();

        QColor color;
        if (item->itemType == "Fixed") {
            color = QColor(0, 0, 0);  // Черный
        } else if (item->itemType == "Source") {
            color = QColor(255, 0, 0);  // Красный
        } else if (item->itemType == "Streamer")
        {
            color = QColor(0, 0, 255);  // Синий
        } else if(item->itemType == "Buoy") {
            color = QColor(0,255,0); //mr green
        } else {
            color = QColor(255,0,255); //mr purple
        }

        int subItemsCount = (item->itemType == "Streamer") ? dynamic_cast<Streamer*>(item)->getChanCount() : 1;
        int totalSteps = queue.size() / subItemsCount;  // Количество шагов для изменения прозрачности
        int step = totalSteps > 1 ? 100 / (totalSteps - 1) : 100;  // Шаг изменения прозрачности


        // Отрисовываем координаты с учетом прозрачности
        int index = 0;
        for (int i = 0; i < queue.size(); i += subItemsCount) {
            int alpha = qMin(100, (step * index)/2);  // Прозрачность от 0 до 100%
            QColor pointColor = color;
            pointColor.setAlpha(alpha * 255 / 100);  // Настраиваем прозрачность

            // Отрисовываем все координаты для текущего шага
            for (int j = 0; j < subItemsCount; ++j) {
                const QPointF& pos = queue[i + j];
                graphicsScene->addEllipse(
                    pos.x() - 2, pos.y() - 2, 4, 4, QPen(Qt::NoPen), QBrush(pointColor)
                    );
            }
            ++index;
        }
    }


}

void UberPainter::collectItems()
{

    for (FixedItem* item: MyVault->ItemsVault) {
        QQueue<QPointF>* curQueue = &recentPositions[item];
        if (item->itemType == "Streamer") {
            Streamer* streamer = dynamic_cast<Streamer*>(item);
            QVector<QPointF> chansPos = getCoor(streamer);
            if (curQueue->size() >= (QUEUES_SIZE*streamer->getChanCount())) {
                for (uint i = 0; i < streamer->getChanCount(); i++) {
                    curQueue->dequeue();
                }
            }
            for (auto i:chansPos) {
                curQueue->enqueue(i);
            }
        } else {
            QPointF itemPos = getCoor(item);
            if (curQueue->size() >= QUEUES_SIZE) curQueue->dequeue();
            curQueue->enqueue(itemPos);
        }
    }

}

QPointF UberPainter::getCoor(FixedItem *item)
{
    return QPointF(item->x_coor, item->y_coor);
}

QVector<QPointF> UberPainter::getCoor(Streamer *streamer)
{
    QVector<QPointF> res(streamer->getChanCount());
    for (uint i = 0; i < streamer->getChanCount(); i++) {
        QPointF chanCoor = QPointF(streamer->getChan(i+1)->x_coor, streamer->getChan(i+1)->y_coor);
        res[i] = chanCoor;
    }
    return res;
}

void UberPainter::createContainer()
{
    bool doWeReallyNeedToReload = false;
    for (auto i: MyVault->ItemsVault) { //
        if (!recentPositions.contains(i)) doWeReallyNeedToReload = true;
    }
    if (doWeReallyNeedToReload) {
        recentPositions.clear();
        for (auto item: MyVault->ItemsVault) {
            QQueue<QPointF> queue;
            recentPositions.insert(item, queue);
        }
    } else {
        return;
    }
}

void UberPainter::eraseFromPainting(FixedItem *toDelete)
{
    recentPositions.erase(recentPositions.find(toDelete));
}
