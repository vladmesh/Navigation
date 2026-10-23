#include "uberpainter.h"

#include "gridmath.h"

#include <algorithm>
#include <mutex>

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
    // Сначала отвязать сцену: деструктор сцены удаляет gridGroup и сам зовёт setScene(nullptr),
    // а пересчёт полос прокрутки дёргает scrollContentsBy → drawGrid с висячим gridGroup.
    setScene(nullptr);
    gridGroup = nullptr; // группой по-прежнему владеет сцена
    delete graphicsScene;
}

void UberPainter::run() //срабатывает по таймеру
{//reading item's position and building map
    collectItems();
    if (isNotDrawingLine) updateView();
    //isDrawingLine - флаг что мы рисуем линии. он снят, пока меряем расстояние от точки до точки на карте
}

void UberPainter::drawGrid()
{
    if (graphicsScene == nullptr || scene() != graphicsScene)
        return; // сцена отвязана (деструктор)

    delete gridGroup; // дети (линии и подписи) удаляются вместе с группой
    gridGroup = nullptr;

    const QRect viewRect = viewport()->rect();
    const QRectF visible = mapToScene(viewRect).boundingRect();
    const double step = GridMath::niceStep(std::min(visible.width(), visible.height()), 10);
    if (step <= 0.0)
        return;

    gridGroup = new QGraphicsItemGroup();
    gridGroup->setZValue(-10); // под треками и объектами
    graphicsScene->addItem(gridGroup);

    // Линии с запасом в один шаг за краями, чтобы при пане не было пустых полос.
    const QRectF area = visible.adjusted(-step, -step, step, step);
    const std::vector<double> xs = GridMath::gridLines(area.left(), area.right(), step);
    const std::vector<double> ys = GridMath::gridLines(area.top(), area.bottom(), step);

    QPen pen(QColor(200, 200, 200));
    pen.setCosmetic(true);
    pen.setWidth(0);
    for (double x : xs)
        (new QGraphicsLineItem(x, area.top(), x, area.bottom(), gridGroup))->setPen(pen);
    for (double y : ys)
        (new QGraphicsLineItem(area.left(), y, area.right(), y, gridGroup))->setPen(pen);

    // Подписи: ItemIgnoresTransformations сам компенсирует scale(1, -1) и зум, поэтому
    // текст не зеркальный и одного размера. Смещение от линии и края — в пикселях
    // (собственный transform такого элемента задаётся в координатах устройства).
    QFont font;
    font.setPointSizeF(8.0);
    const int decimals = GridMath::labelDecimals(step);
    const QPointF bottomLeft = mapToScene(viewRect.bottomLeft()); // X левого края, Y нижнего
    const int marginPx = 3;
    auto addLabel = [&](double value, const QPointF& anchor, bool alongBottom) {
        const double shown = (value == 0.0) ? 0.0 : value; // без "-0"
        auto* label = new QGraphicsSimpleTextItem(QString::number(shown, 'f', decimals), gridGroup);
        label->setFont(font);
        label->setBrush(QColor(90, 90, 90));
        label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        label->setPos(anchor);
        const QRectF box = label->boundingRect();
        // X: справа от линии над нижним краем; Y: над линией правее левого края.
        const QPointF offset(marginPx, alongBottom ? -box.height() - marginPx : -box.height());
        label->setTransform(QTransform::fromTranslate(offset.x(), offset.y()));

        // Обрезанная краем подпись читается как другое число, а Y-подпись у нижнего края
        // наезжает на ряд X-подписей: такие не показываем (линия остаётся).
        const QPointF topLeft = QPointF(mapFromScene(anchor)) + offset;
        const bool clipped = topLeft.x() < 0 || topLeft.y() < 0 || topLeft.x() + box.width() > viewRect.width() ||
                             topLeft.y() + box.height() > viewRect.height();
        const bool overXRow =
            !alongBottom && topLeft.y() + box.height() > viewRect.height() - box.height() - 2 * marginPx;
        if (clipped || overXRow)
            delete label;
    };
    for (double x : xs) {
        if (x >= visible.left() && x <= visible.right())
            addLabel(x, QPointF(x, bottomLeft.y()), true);
    }
    for (double y : ys) {
        if (y >= visible.top() && y <= visible.bottom())
            addLabel(y, QPointF(bottomLeft.x(), y), false);
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

    QGraphicsView::mouseMoveEvent(event); // сетку при пане перестраивает scrollContentsBy
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

    QMutexLocker locker(&sceneMutex);
    drawGrid(); // шаг меняется сразу, без ожидания таймера
}

void UberPainter::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    // Любой сдвиг вида: пан мышью, полосы прокрутки, клавиши. Внутри updateView()
    // setSceneRect/centerOn могут прокрутить вид синхронно под захваченным sceneMutex —
    // тогда пропускаем, updateView() рисует сетку сам.
    std::unique_lock<QMutex> lock(sceneMutex, std::try_to_lock);
    if (lock.owns_lock())
        drawGrid();
}

void UberPainter::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    // Ресайз вьюпорта может прийти синхронно изнутри updateView() (setSceneRect меняет
    // полосы прокрутки), где sceneMutex уже захвачен. Повторно его не берём: updateView()
    // сам нарисует сетку после setSceneRect.
    std::unique_lock<QMutex> lock(sceneMutex, std::try_to_lock);
    if (lock.owns_lock())
        drawGrid();
}

void UberPainter::updateView()
{
    centerView();
    QMutexLocker locker(&sceneMutex);
    graphicsScene->clear();
    gridGroup = nullptr; // clear() уже удалил группу

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

    drawGrid();

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
