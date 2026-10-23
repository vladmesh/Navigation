#include "itemsstorage.h"


ItemsStorage::ItemsStorage() {

    ItemsVault.reserve(20);
    //reservation of dynamic memory for 20 elements
}

ItemsStorage::~ItemsStorage() {
    deleteAllItems();
    ItemsVault.clear();
}

void ItemsStorage::SaveItem(FixedItem* NewItem){
    ItemsVault.push_back(NewItem);
    // connect
    // qDebug() <<"I am in Vault "<<NewItem->itemType<<NewItem->name;

}

void ItemsStorage::DeleteItem(QString Name) {
    for (auto it = ItemsVault.begin(); it != ItemsVault.end(); it++) {
        qDebug() <<(*it)->name;
    }
    // return;
    for (auto it = ItemsVault.begin(); it != ItemsVault.end(); it++) {
        if ((*it) == nullptr) {
            delete *it;
            ItemsVault.erase(it);
            break;
        }
        if ((*it)->name == Name) {
            delete *it;
            ItemsVault.erase(it);
            break;
        }
    }
    // for (auto it = ItemsVault.begin(); it != ItemsVault.end(); it++) {
    //     if ((*it)->IShouldBeDeleted) {
    //         delete *it;
    //         ItemsVault.erase(it);
    //         break;
    //     }
    // }
    for (auto it = ItemsVault.begin(); it != ItemsVault.end(); it++) {
        qDebug() <<(*it)->name;
    }
    // qDebug();
}
void ItemsStorage::setItemForCalculation() {
    setItemForCalculation(this->ItemsVault);
}
void ItemsStorage::setItemForCalculation(QVector<QPointer<FixedItem>> ItemsVault)
{
    QPointer<FixedItem> itemForCalculation = nullptr;
    for (auto i: ItemsVault) {
        if (i->itemType == "Fixed") {
            if (i->hasConnection || i->connection) {
                itemForCalculation = i;
                i->amIItemForCalculating = true;
                break;
            }
        }
    }
    if (itemForCalculation == nullptr) {
        for (auto i: ItemsVault) {
            if (i->hasConnection) {
                itemForCalculation = i;
                break;
            }
        }
        if (itemForCalculation == nullptr) {
            qDebug() << __FUNCTION__ << "no item with good connection";
            return;
        }
    }
    for (auto i: ItemsVault) {
        if (i->itemType == "Fixed" && !i->hasConnection) {
            i->setItemForCalculations(itemForCalculation);
        }
    }
}

void ItemsStorage::sortItemsByCalcPriority()
{
    for (auto i: ItemsVault) {
        i->calculationPriority = -1;
    }
    for (auto i: ItemsVault) {
        if (i->hasConnection || i->connection != nullptr) {
            i->calculationPriority = 0;
        }
    }
    for (auto i: ItemsVault) {
        if (!(i->hasConnection || i->connection != nullptr) && i->itemType == "Fixed") {
            i->calculationPriority = 1;
        }
    }
    int stopFlag = ItemsVault.size();
    while(stopFlag >= 0) {
        for (auto i: ItemsVault) {
            if (!(i->hasConnection || i->connection != nullptr) && i->itemType != "Fixed") {
                QPointer<TowedItem> towedI = QPointer(qobject_cast<TowedItem*>(i));
                if (towedI->towingPoint->calculationPriority >= 0) {
                    if (i->itemType != "Streamer") {
                        towedI->calculationPriority = towedI->towingPoint->calculationPriority + 2;
                    } else {
                        towedI->calculationPriority = towedI->towingPoint->calculationPriority + 1;
                    }

                }
            }
            stopFlag--;
        }
    }
    for (int i = 0; i < ItemsVault.size(); i++) {
        for (int j = 0; j < ItemsVault.size()-1; j++) {
            if (ItemsVault[j]->calculationPriority > ItemsVault[j+1]->calculationPriority) {
                std::swap(ItemsVault[j], ItemsVault[j+1]);
            }
        }
    }
    for (auto i: ItemsVault) {
        qDebug() << i->name << i->calculationPriority << __FUNCTION__;
    }
}

void ItemsStorage::setItemsCreationIndex(QVector<QPointer<FixedItem>> &ItemsVault)
{
    FixedItem* itemForCalculation = nullptr;

    QVector<QPointer<FixedItem>> firstPriority;
    QVector<QPointer<FixedItem>> secondPriority;
    QVector<QPointer<TowedItem>> lastPriority;


    for (auto i: ItemsVault) { //разделение объектов по приоритету создания (см. объяснение хэддер FixedItem)
        qDebug() << i->name << i->creationPriority;
        if (QString(i->metaObject()->className()) == "FixedItem") {
            if (i->hasConnection && i->connection != nullptr) {
                itemForCalculation = i;
                firstPriority.push_back(i);
                i->creationPriority = 0;
            } else {
                secondPriority.push_back(i);
                i->creationPriority = 1;
            }
        } else {
            lastPriority.push_back(QPointer(qobject_cast<TowedItem*>(i)));
            // lastPriority.push_back(QPointer(dynamic_cast<TowedItem*>(i)));
        }
    }

    int timeBreakerFlag = 0;
    while (timeBreakerFlag < lastPriority.size() + 1) {
        for (auto i: lastPriority) {
            if (i->towingPoint->creationPriority == -1) {
                continue;
            } else {
                i->creationPriority = i->towingPoint->creationPriority + 2;
            }
        }
        timeBreakerFlag++;
    }
    // for (auto i: ItemsVault) {
    //     qDebug() << i->name << i->creationPriority;
    // }

}

void ItemsStorage::deleteAllItems()
{
    for (auto i: ItemsVault) {
        if (i) delete i;
    }
}

void ItemsStorage::cleanVector()
{
    ItemsVault.erase(std::remove_if(ItemsVault.begin(), ItemsVault.end(),
        [](const QPointer<FixedItem>& ptr) { return ptr.isNull(); }),
        ItemsVault.end());
}

FixedItem* ItemsStorage::getItem(QString name) {
    for (auto it = ItemsVault.begin(); it != ItemsVault.end(); it++) {
        if((*it)->name == name) {
            return *it;
        }
    }
    qDebug() << "No Such Item";
    return nullptr;
}
