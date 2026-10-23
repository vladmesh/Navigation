#include "coordinator.h"

Coordinator::Coordinator(QObject *parent)
    : QObject{parent}
{}

int Coordinator::calcCoors()
{
    try {
        for (auto item: Vault->ItemsVault) {
            if (item == nullptr) {
                // for (auto i = Vault->ItemsVault.begin(); i != Vault->ItemsVault.end(); ++i) {
                //     Vault->ItemsVault.erase(i);
                // }
                continue;
            }
            if (item->needCalculate == false) return 2;
            if (item->hasConnection) {
                if (item->connection == nullptr) {

                }
                item->getLastGGA();

                if (NmeaParser::isValid(item->lastGGAData)) {
                    item->calcItemCoordinates();
                }
            }
        }
        // qDebug() <<__FUNCTION__<<__LINE__;
        Vault->setItemForCalculation(Vault->ItemsVault);
        // qDebug() <<__FUNCTION__<<__LINE__;

        for (FixedItem* item: Vault->ItemsVault) { //я вызываю метод предка
            if (item == nullptr) continue;

            if (item->hasConnection) continue;
            // qDebug() << item->name << item->hasConnection;
            QString className = QString(item->metaObject()->className());

            if (className == "Streamer") {
                item->calcItemCoordinates();
                Streamer* tmp = dynamic_cast<Streamer*>(item);
                tmp->calcChansCoors();
                // tmp->calcStreamerDepth();s
            } else if(className == "FixedItem") {
                item->calcItemCoordinates();
            } else if (className == "TowedItem") {
                dynamic_cast<TowedItem*>(item)->calcItemCoordinates();
            } else if (className == "Buoy") { //for buoy
                dynamic_cast<TowedItem*>(item)->calcItemCoordinates();
            } else if (className == "Source") { //for Source
                dynamic_cast<TowedItem*>(item)->calcItemCoordinates();

            }

            if (item->azimuthOfMovement == -1) {
                //if we don't have azimuth, we get it from someone else
                item->azimuthOfMovement = commonDummyAzimuth;
            }
        }
    } catch (std::exception& e) {
        qDebug() << e.what();
        return 0;
    }
    return 1;
}

int Coordinator::printCoors()
{
    try{
        for (FixedItem* item: Vault->ItemsVault) {
            QString className = QString(item->metaObject()->className());
            if (className == "Streamer") {
                dynamic_cast<Streamer*>(item)->printPos();
            } else {
                item->printPos();
            }
        }
    } catch(std::exception& e) {
        qDebug() << e.what();
        return 0;
    }
    return 1;
}

bool Coordinator::wireFixedItems()
{
    FixedItem* ItemWithConnection = nullptr;
    QString className;
    for (FixedItem* item: Vault->ItemsVault){
        className = QString(item->metaObject()->className());
        if (className == "FixedItem") {
            if (item->hasConnection && item->connection != nullptr) {
                ItemWithConnection = item;
                break;
            }
        }
    }
    if (ItemWithConnection == nullptr) {
        return false;
    } else {
        for (FixedItem* item: Vault->ItemsVault){
            if(item->hasConnection) continue;
            className = QString(item->metaObject()->className());
            if (className == "FixedItem") {
                if (!item->hasConnection) {
                    qDebug() << item->name << " wired to " << ItemWithConnection->name;
                    item->ItemForCalculations = ItemWithConnection;
                }
            }
        }
    }
    return true;
}

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


void Coordinator::boardDepthChanged(double boardHeight)
{
    for (FixedItem* item: Vault->ItemsVault) {
        if (item->itemType == "Fixed") continue;
        dynamic_cast<TowedItem*>(item)->setBoardHeight(boardHeight);
    }
    qDebug() << "Coordinator::boardDepthChanged to" << boardHeight;
}


