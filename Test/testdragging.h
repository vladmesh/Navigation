#ifndef TESTDRAGGING_H
#define TESTDRAGGING_H

#include <QObject>
#include "test_postprocessing.h"


class TestDragging : public QObject
{
    Q_OBJECT
public:
    TestDragging();

    void testChainAlgo1();
    QVector<FixedItem *> initItems1();
};

#endif // TESTDRAGGING_H
