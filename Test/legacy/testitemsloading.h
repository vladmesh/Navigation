#ifndef TEST_ITEMSLOADING_H
#define TEST_ITEMSLOADING_H

#include "fabric.h"
#include "itemsstorage.h"
#include "itemsloader.h"


class testItemsLoading
{
public:
    testItemsLoading();

    void runTests();

    void itemsToJson();
    void jsonToItems();

    QVector<QPointer<FixedItem>> createItems2309();

private:
    ItemsLoader loader;
};

#endif // TEST_ITEMSLOADING_H


