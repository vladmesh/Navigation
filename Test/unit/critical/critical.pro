# QtTest на исправления C1-C3 (PostProcessor, Connection::calcQuality,
# ItemsLoader). Отдельно от Ship.pro, полевые данные не нужны.
# Запуск без дисплея: QT_QPA_PLATFORM=offscreen ./tst_critical
QT += testlib core gui network serialport location positioning widgets
CONFIG += c++17 console testcase
CONFIG -= app_bundle
TARGET = tst_critical

include(../../app_sources.pri)

SOURCES += tst_critical.cpp
