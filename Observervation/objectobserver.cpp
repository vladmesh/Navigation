#include "objectobserver.h"

ObjectObserver::ObjectObserver() {}

ObjectObserver::ObjectObserver(ItemsStorage *Vault) : Vault(Vault)
{
    checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, &ObjectObserver::timeToCheckState);
    checkTimer->start(5000);
}

ObjectObserver::~ObjectObserver() {}

void ObjectObserver::react()
{

}

void ObjectObserver::statusChanged()
{
    react();
}

void ObjectObserver::timeToCheckState()
{
    for (auto i: Vault->ItemsVault) {
        if (!i->hasConnection || i->connection.isNull()) continue;
        hasConnectionChanged(i);
    }
}

void ObjectObserver::hasConnectionChanged(FixedItem *who)
{
    QString name = who->name;
    for (int row = 0; row < tableWithItems->rowCount(); ++row) {
        QTableWidgetItem *firstItem = tableWithItems->item(row, 0);  // Первый столбец
        if (firstItem && firstItem->text() == name) {
            QTableWidgetItem *fourthItem = tableWithItems->item(row, 4);  // Пятый столбец
            if (who->hasConnection) {
                fourthItem->setBackground(QColor(Qt::green));
                fourthItem->setText("GOOD");
            } else {
                fourthItem->setBackground(QColor(Qt::red));
                fourthItem->setText("BAD");
            }
        }
    }
}

void ObjectObserver::itemAdded(FixedItem *who)
{
    if (who == nullptr) return;
    connect(who, &FixedItem::hasConnectionChanged,this, &ObjectObserver::hasConnectionChanged);
}


void ObjectObserver::setTableWithItems(QTableWidget *newTableWithItems)
{
    tableWithItems = newTableWithItems;
}
