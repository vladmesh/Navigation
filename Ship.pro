QT += core gui network serialport location core5compat positioning
#
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += ./Drawing \
./Fabric \
./Objects \
./DataVault \
./ConnectionCreator \
./Connections \
./Parsers \
./Service \
./Data \
./Data/Params \
./DataReader \
./Test \
./Observervation \
./MenuWindows


SOURCES += \
    Connections/connectionlogic.cpp \
    Connections/connectionui.cpp \
    Connections/sender.cpp \
    DataVault/itemsloader.cpp \
    DataVault/itemsstorage.cpp \
    # Drawing/lordofgraphic.cpp \
    Drawing/uberpainter.cpp \
    Fabric/buoydialog.cpp \
    Fabric/fabric.cpp \
    Fabric/streamerdialog.cpp \
    MenuWindows/p190headeranalyzer.cpp \
    MenuWindows/p190headerprocessor.cpp \
    MenuWindows/sendersettingsmenu.cpp \
    Objects/buoy.cpp \
    Objects/fixeditem.cpp \
    Objects/source.cpp \
    Objects/streamer.cpp \
    Objects/toweditem.cpp \
    DataReader/segyreader.cpp \
    Observervation/objectobserver.cpp \
    Observervation/virtualobserver.cpp \
    Parsers/ppkparser.cpp \
    Service/coordinator.cpp \
    MenuWindows/p190headerchanger.cpp \
    Service/postprocessor.cpp \
    Service/postprocessorview.cpp \
    Test/test_postprocessing.cpp \
    Test/testdragging.cpp \
    Test/testitemsloading.cpp \
    Test/testppkparser.cpp \
    main.cpp \
    mainwindow.cpp \
    Drawing/mygraphicview.cpp \
    Connections/connection_com.cpp \
    Connections/connection_net.cpp \
    Parsers/nmeaparser.cpp \
    Parsers/p190_creator.cpp \
    ConnectionCreator/connection_creator.cpp

HEADERS += \
    Connections/connectionlogic.h \
    Connections/connectionui.h \
    Connections/sender.h \
    DataVault/CircularBuffer.h \
    DataVault/itemsloader.h \
    DataVault/itemsstorage.h \
    DataVault/structures.h \
    Drawing/hoverpaintingrpoint.h \
    # Drawing/lordofgraphic.h \
    Drawing/uberpainter.h \
    Fabric/buoydialog.h \
    Fabric/fabric.h \
    Fabric/sourcedialog.h \
    Fabric/streamerdialog.h \
    MenuWindows/p190headeranalyzer.h \
    MenuWindows/p190headerprocessor.h \
    MenuWindows/sendersettingsmenu.h \
    MenuWindows/shipparamschanger.h \
    Objects/buoy.h \
    Objects/fixeditem.h \
    Objects/source.h \
    Objects/streamer.h \
    Objects/toweditem.h \
    DataReader/segyreader.h \
    Observervation/objectobserver.h \
    Observervation/virtualobserver.h \
    Parsers/ppkparser.h \
    Service/coordinator.h \
    Service/customdoublespinbox.h \
    Service/functions.h \
    MenuWindows/p190headerchanger.h \
    Service/postprocessor.h \
    Service/postprocessorview.h \
    Test/test_postprocessing.h \
    Test/testdragging.h \
    Test/testitemsloading.h \
    Test/testppkparser.h \
    mainwindow.h \
    Drawing/mygraphicview.h \
    ConnectionCreator/connection_creator.h \
    Connections/connection_com.h \
    Connections/connection_net.h \
    Parsers/nmeaparser.h \
    Parsers/p190_creator.h

FORMS += \
    Connections/connectionui.ui \
    mainwindow.ui \
    ConnectionCreator/connection_creator.ui \
    Connections/connection_com.ui \
    Connections/connection_net.ui





# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += resources.qrc
win32:RC_FILE = resources_2.rc

# После сборки release-версии под Windows разворачиваем рядом с .exe все
# нужные Qt-библиотеки и плагины через windeployqt, иначе приложение не
# запустится на машине без установленного Qt.
# CONFIG+=no_windeployqt отключает этот шаг (CI smoke-e2e проверяет набор
# из deploy/win64-mingw, windeployqt не должен докладывать свои файлы).
win32:!no_windeployqt {
    CONFIG(release, debug|release) {
        WINDEPLOYQT = $$shell_path($$[QT_INSTALL_BINS]/windeployqt.exe)
        DEPLOY_TARGET = $$shell_path($$OUT_PWD/release/$${TARGET}.exe)
        QMAKE_POST_LINK += $$WINDEPLOYQT $$DEPLOY_TARGET
    }
}
