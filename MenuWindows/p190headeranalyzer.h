#ifndef P190HEADERANALYZER_H
#define P190HEADERANALYZER_H

#include <QObject>
#include "DataVault/itemsstorage.h"


class p190HeaderAnalyzer : public QObject
{
    Q_OBJECT
public:
    p190HeaderAnalyzer();


    bool analizeVault();


    int numOfBuoys = 0;
    int numOfSources = 0;
    int numOfStreamers = 0;

    void setIndexForSpecialItem(QPointer<FixedItem> item);
    static QString getHeaderStringFromItem(QPointer<FixedItem> item);

    ItemsStorage* Vault;
    void setVault(ItemsStorage* newVault);
};

#endif // P190HEADERANALYZER_H
