// Archived from Service/coordinator.cpp / coordinator.h. Commit: 4ac56dd (Initial import), identical in main aee8ae8.
// Feature: refine the source position by gradient descent over distances to
// receivers (multilateration). Never called; where the distances would come
// from (acoustics, first breaks) is not defined anywhere in the code.
// Status: algorithm only, no integration.
//
// Declarations (class Coordinator, public):
//     /**
//      * @brief Обновление позиции источника. Пока не используется
//      * Использует градиентный спуск для вычисления положения источника на основе позиций приемников и их расстояний.
//      * @param sCoor Начальное положение источника.
//      * @param rCoors Координаты приемников.
//      * @param dist Расстояния до источника.
//      * @return Обновленные координаты источника.
//      */
//     QPointF updateSourcePosition(QPointF sCoor, QVector<QPointF> rCoors, QVector<double> dist);
//     /**
//      * @brief Вычисление ошибки между расчетными и заданными расстояниями.
//      * Пока не используется
//      * @param source Координаты источника.
//      * @param receivers Координаты приемников.
//      * @param distances Расстояния до источника.
//      * @return Общая ошибка.
//      */
//     double calculateError(const QPointF &source, const QVector<QPointF> &receivers, const QVector<double> &distances);

QPointF Coordinator::updateSourcePosition(QPointF sCoor, QVector<QPointF> rCoors, QVector<double> dist)
{
    double learningRate = 0.01; // Скорость обучения для градиентного спуска
    int maxIterations = 100;  // Максимальное количество итераций
    double tolerance = 0.05;    // Допустимая ошибка для остановки

    QPointF source = sCoor;  // Начальная позиция источника
    double lastError = calculateError(source, rCoors, dist);

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        double gradX = 0.0;
        double gradY = 0.0;

        // Вычисляем градиенты по x и y
        for (int i = 0; i < rCoors.size(); ++i) {
            double calculatedDistance = qSqrt(qPow(source.x() - rCoors[i].x(), 2) + qPow(source.y() - rCoors[i].y(), 2));
            if (calculatedDistance == 0) continue; // Избегаем деления на ноль

            double difference = calculatedDistance - dist[i];
            gradX += 2 * difference * (source.x() - rCoors[i].x()) / calculatedDistance;
            gradY += 2 * difference * (source.y() - rCoors[i].y()) / calculatedDistance;
        }

        // Обновляем позицию источника с использованием градиентного спуска
        source.setX(source.x() - learningRate * gradX);
        source.setY(source.y() - learningRate * gradY);

        // Вычисляем новую ошибку и проверяем условие остановки
        double currentError = calculateError(source, rCoors, dist);
        if (qAbs(currentError - lastError) < tolerance) {
            break; // Останавливаемся, если улучшение меньше допустимого значения
        }
        lastError = currentError;
    }

    return source;
}

double Coordinator::calculateError(const QPointF &source, const QVector<QPointF> &receivers, const QVector<double> &distances)
{
    double error = 0.0;
    for (int i = 0; i < receivers.size(); ++i) {
        double calculatedDistance = qSqrt(qPow(source.x() - receivers[i].x(), 2) + qPow(source.y() - receivers[i].y(), 2));
        error += qPow(calculatedDistance - distances[i], 2);
    }
    return error;
}
