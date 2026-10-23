# Unit-тест GridMath (ADR 0001, Task 01). Только QtCore + QtTest, без GUI.
#   qmake Test/unit/gridmath/tst_gridmath.pro && make && ./tst_gridmath
QT = core testlib
CONFIG += c++17 console testcase warn_on
CONFIG -= app_bundle

TARGET = tst_gridmath
TEMPLATE = app

GRIDMATH_DIR = $$PWD/../../../Drawing
INCLUDEPATH += $$GRIDMATH_DIR

SOURCES += \
    tst_gridmath.cpp \
    $$GRIDMATH_DIR/gridmath.cpp

HEADERS += \
    $$GRIDMATH_DIR/gridmath.h
