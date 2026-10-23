# Автономные юнит-тесты (QtTest) для парсеров NMEA/PPK.
# Не зависят от GUI и от полевых данных, собираются на Windows и Linux,
# запускаются в CI (в т.ч. под ASan/UBSan).
QT += testlib core gui widgets positioning
CONFIG += c++17 console testcase
CONFIG -= app_bundle
TARGET = tst_parsers

ROOT = $$PWD/../..
INCLUDEPATH += $$ROOT/Parsers

SOURCES += \
    tst_parsers.cpp \
    $$ROOT/Parsers/nmeaparser.cpp \
    $$ROOT/Parsers/ppkparser.cpp

HEADERS += \
    $$ROOT/Parsers/nmeaparser.h \
    $$ROOT/Parsers/ppkparser.h
