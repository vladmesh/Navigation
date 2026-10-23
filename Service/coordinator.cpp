#include "coordinator.h"

Coordinator::Coordinator(QObject *parent)
    : QObject{parent}
{}

int Coordinator::calcCoors()
{
    try {
        for (auto item: Vault->ItemsVault) {
            if (item == nullptr) {
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
        Vault->setItemForCalculation(Vault->ItemsVault);

        for (FixedItem* item: Vault->ItemsVault) { //я вызываю метод предка
            if (item == nullptr) continue;

            if (item->hasConnection) continue;
            QString className = QString(item->metaObject()->className());

            if (className == "Streamer") {
                item->calcItemCoordinates();
                Streamer* tmp = dynamic_cast<Streamer*>(item);
                tmp->calcChansCoors();
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


void Coordinator::boardDepthChanged(double boardHeight)
{
    for (FixedItem* item: Vault->ItemsVault) {
        if (item->itemType == "Fixed") continue;
        dynamic_cast<TowedItem*>(item)->setBoardHeight(boardHeight);
    }
    qDebug() << "Coordinator::boardDepthChanged to" << boardHeight;
}


