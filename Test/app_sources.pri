# Исходники приложения без main.cpp и mainwindow.*, для тестовых проектов
# (Test/legacy, Test/unit). Список повторяет Ship.pro: при добавлении файла
# в Ship.pro добавить его и сюда.
NAV_ROOT = $$PWD/..

INCLUDEPATH += \
    $$NAV_ROOT \
    $$NAV_ROOT/Drawing \
    $$NAV_ROOT/Fabric \
    $$NAV_ROOT/Objects \
    $$NAV_ROOT/DataVault \
    $$NAV_ROOT/ConnectionCreator \
    $$NAV_ROOT/Connections \
    $$NAV_ROOT/Parsers \
    $$NAV_ROOT/Service \
    $$NAV_ROOT/Data \
    $$NAV_ROOT/DataReader \
    $$NAV_ROOT/Observervation \
    $$NAV_ROOT/MenuWindows

SOURCES += \
    $$NAV_ROOT/Connections/connectionlogic.cpp \
    $$NAV_ROOT/Connections/connectionui.cpp \
    $$NAV_ROOT/Connections/sender.cpp \
    $$NAV_ROOT/DataVault/itemsloader.cpp \
    $$NAV_ROOT/DataVault/itemsstorage.cpp \
    $$NAV_ROOT/Drawing/uberpainter.cpp \
    $$NAV_ROOT/Fabric/buoydialog.cpp \
    $$NAV_ROOT/Fabric/fabric.cpp \
    $$NAV_ROOT/Fabric/streamerdialog.cpp \
    $$NAV_ROOT/MenuWindows/p190headeranalyzer.cpp \
    $$NAV_ROOT/MenuWindows/p190headerprocessor.cpp \
    $$NAV_ROOT/MenuWindows/sendersettingsmenu.cpp \
    $$NAV_ROOT/Objects/buoy.cpp \
    $$NAV_ROOT/Objects/fixeditem.cpp \
    $$NAV_ROOT/Objects/source.cpp \
    $$NAV_ROOT/Objects/streamer.cpp \
    $$NAV_ROOT/Objects/toweditem.cpp \
    $$NAV_ROOT/DataReader/segyreader.cpp \
    $$NAV_ROOT/Observervation/objectobserver.cpp \
    $$NAV_ROOT/Observervation/virtualobserver.cpp \
    $$NAV_ROOT/Parsers/ppkparser.cpp \
    $$NAV_ROOT/Service/coordinator.cpp \
    $$NAV_ROOT/MenuWindows/p190headerchanger.cpp \
    $$NAV_ROOT/Service/postprocessor.cpp \
    $$NAV_ROOT/Service/postprocessorview.cpp \
    $$NAV_ROOT/Drawing/mygraphicview.cpp \
    $$NAV_ROOT/Connections/connection_com.cpp \
    $$NAV_ROOT/Connections/connection_net.cpp \
    $$NAV_ROOT/Parsers/nmeaparser.cpp \
    $$NAV_ROOT/Parsers/p190_creator.cpp \
    $$NAV_ROOT/ConnectionCreator/connection_creator.cpp

HEADERS += \
    $$NAV_ROOT/Connections/connectionlogic.h \
    $$NAV_ROOT/Connections/connectionui.h \
    $$NAV_ROOT/Connections/sender.h \
    $$NAV_ROOT/DataVault/CircularBuffer.h \
    $$NAV_ROOT/DataVault/itemsloader.h \
    $$NAV_ROOT/DataVault/itemsstorage.h \
    $$NAV_ROOT/DataVault/structures.h \
    $$NAV_ROOT/Drawing/hoverpaintingrpoint.h \
    $$NAV_ROOT/Drawing/uberpainter.h \
    $$NAV_ROOT/Fabric/buoydialog.h \
    $$NAV_ROOT/Fabric/fabric.h \
    $$NAV_ROOT/Fabric/sourcedialog.h \
    $$NAV_ROOT/Fabric/streamerdialog.h \
    $$NAV_ROOT/MenuWindows/p190headeranalyzer.h \
    $$NAV_ROOT/MenuWindows/p190headerprocessor.h \
    $$NAV_ROOT/MenuWindows/sendersettingsmenu.h \
    $$NAV_ROOT/MenuWindows/shipparamschanger.h \
    $$NAV_ROOT/Objects/buoy.h \
    $$NAV_ROOT/Objects/fixeditem.h \
    $$NAV_ROOT/Objects/source.h \
    $$NAV_ROOT/Objects/streamer.h \
    $$NAV_ROOT/Objects/toweditem.h \
    $$NAV_ROOT/DataReader/segyreader.h \
    $$NAV_ROOT/Observervation/objectobserver.h \
    $$NAV_ROOT/Observervation/virtualobserver.h \
    $$NAV_ROOT/Parsers/ppkparser.h \
    $$NAV_ROOT/Service/coordinator.h \
    $$NAV_ROOT/Service/customdoublespinbox.h \
    $$NAV_ROOT/Service/functions.h \
    $$NAV_ROOT/MenuWindows/p190headerchanger.h \
    $$NAV_ROOT/Service/postprocessor.h \
    $$NAV_ROOT/Service/postprocessorview.h \
    $$NAV_ROOT/Drawing/mygraphicview.h \
    $$NAV_ROOT/ConnectionCreator/connection_creator.h \
    $$NAV_ROOT/Connections/connection_com.h \
    $$NAV_ROOT/Connections/connection_net.h \
    $$NAV_ROOT/Parsers/nmeaparser.h \
    $$NAV_ROOT/Parsers/p190_creator.h

FORMS += \
    $$NAV_ROOT/Connections/connectionui.ui \
    $$NAV_ROOT/ConnectionCreator/connection_creator.ui

RESOURCES += $$NAV_ROOT/resources.qrc
