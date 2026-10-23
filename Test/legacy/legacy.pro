# Ручные сценарии на полевых данных автора (бывшие Test/* из Ship.pro).
# Собираются отдельным бинарём и в релиз Ship не попадают.
# Пути к данным: переменная окружения NAV_TEST_DATA_DIR (см. testdatapath.h).
QT += core gui network serialport location positioning widgets
CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = nav_legacy_tests

include(../app_sources.pri)

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    test_postprocessing.cpp \
    testitemsloading.cpp \
    testppkparser.cpp

HEADERS += \
    testdatapath.h \
    test_postprocessing.h \
    testitemsloading.h \
    testppkparser.h
