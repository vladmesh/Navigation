#include "mainwindow.h"
#include "test_postprocessing.h"
#include "testitemsloading.h"
#include "testppkparser.h"

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QScreen>

QPalette getLightPalette();
QPalette getToxicPalette();
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // testPostprocessing test;
    // return 0;

    a.setStyle("Fusion");
    a.setWindowIcon(QIcon(":/Data/icon.ico"));
    QApplication::setWindowIcon(QIcon(":/Data/icon.ico"));
    QPalette goodPalette = getLightPalette();
    QPalette toxicPalette = getToxicPalette();
    a.setPalette(goodPalette);

    if (QDate::currentDate().year() > 2027) {
        QMessageBox msgBox;
        msgBox.critical(nullptr, "","To continue using the application, an update is required. Please download the latest version");
        exit(0);
    }


    QPixmap logo(":/Data/logoGeodevice.svg");
    QSplashScreen splash(logo);
    splash.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint); // Убираем фон и рамки
    splash.show();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    // Центрируем splash screen по экрану
    int x = (screenGeometry.width() - splash.width()) / 2;
    int y = (screenGeometry.height() - splash.height()) / 2;
    splash.move(x, y);

    MainWindow w;
    QTimer::singleShot(1000, &splash, &QSplashScreen::close); // Закрыть заставку через 3 секунды
    QTimer::singleShot(1000, &w, [&]() { w.show(); });

    // w.show();

    return a.exec();
}


QPalette getLightPalette() {
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::WindowText, Qt::black);
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
    lightPalette.setColor(QPalette::ToolTipText, Qt::black);
    lightPalette.setColor(QPalette::Text, Qt::black);
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, Qt::black);
    lightPalette.setColor(QPalette::BrightText, Qt::red);
    return lightPalette;
}
QPalette getToxicPalette() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 0, 255));          // Яркий розовый для фона окон
    palette.setColor(QPalette::WindowText, QColor(0, 255, 0));        // Яркий зеленый для текста
    palette.setColor(QPalette::Base, QColor(0, 255, 255));            // Голубой для фона виджетов (например, полей ввода)
    palette.setColor(QPalette::AlternateBase, QColor(255, 255, 0));   // Желтый для альтернативного фона
    palette.setColor(QPalette::ToolTipBase, QColor(255, 0, 0));       // Красный для подсказок
    palette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));   // Белый для текста подсказок
    palette.setColor(QPalette::Text, QColor(255, 165, 0));            // Оранжевый для обычного текста
    palette.setColor(QPalette::Button, QColor(0, 0, 255));            // Яркий синий для кнопок
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));    // Белый для текста на кнопках
    palette.setColor(QPalette::BrightText, QColor(255, 0, 0));        // Ярко-красный для выделенного текста
    return palette;
}
