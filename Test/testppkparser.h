#ifndef TESTPPKPARSER_H
#define TESTPPKPARSER_H

#include <QObject>
#include <QFile>
#include "nmeaparser.h"
#include "ppkparser.h"


class testPpkParser
{
public:
    testPpkParser();

    void readPpk();

    void compareWithNmea();
};

#endif // TESTPPKPARSER_H
