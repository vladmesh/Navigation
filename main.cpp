#include "mainwindow.h"
#include "test_postprocessing.h"
#include "testitemsloading.h"
#include "testppkparser.h"

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QScreen>
#include <QImage>
#include <QTimer>
#include <cstdio>

QPalette getLightPalette();
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    a.setStyle("Fusion");
    a.setWindowIcon(QIcon(":/Data/icon.ico"));
    QApplication::setWindowIcon(QIcon(":/Data/icon.ico"));
    QPalette goodPalette = getLightPalette();
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


    // --smoke: CI self-check. Exit after 3 s with code 0 if the splash logo (SVG plugin),
    // the icon (ICO plugin) and the main window are OK, otherwise with code 1.
    if (QCoreApplication::arguments().contains(QStringLiteral("--smoke"))) {
        QTimer::singleShot(3000, &a, [&]() {
            const bool svgOk = !logo.isNull();
            const bool icoOk = !QImage(QStringLiteral(":/Data/icon.ico")).isNull();
            const bool windowOk = w.isVisible() && !splash.isVisible();
            fprintf(stderr, "SMOKE platform=%s svg=%d ico=%d window=%d\n", qPrintable(QGuiApplication::platformName()),
                    svgOk, icoOk, windowOk);
            const bool ok = svgOk && icoOk && windowOk;
            fprintf(stderr, ok ? "SMOKE OK\n" : "SMOKE FAIL\n");
            fflush(stderr);
            a.exit(ok ? 0 : 1);
        });
    }

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
