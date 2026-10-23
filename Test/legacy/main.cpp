// Ручные сценарии на полевых данных. Не QtTest: результат смотрится глазами
// по логу и выходным файлам. Запуск:
//   NAV_TEST_DATA_DIR=<корень данных> ./nav_legacy_tests <scenario>
// scenario: postprocessing | itemsloading | ppk
#include <QApplication>
#include <QTextStream>

#include "test_postprocessing.h"
#include "testitemsloading.h"
#include "testppkparser.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    const QString scenario = argc > 1 ? QString::fromLocal8Bit(argv[1]) : QString();

    if (scenario == QLatin1String("postprocessing")) {
        testPostprocessing t;
    } else if (scenario == QLatin1String("itemsloading")) {
        testItemsLoading t;
    } else if (scenario == QLatin1String("ppk")) {
        testPpkParser t;
    } else {
        QTextStream(stderr) << "usage: nav_legacy_tests postprocessing|itemsloading|ppk\n"
                               "field data root: NAV_TEST_DATA_DIR\n";
        return 2;
    }
    return 0;
}
