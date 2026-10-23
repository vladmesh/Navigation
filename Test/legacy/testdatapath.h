#ifndef TESTDATAPATH_H
#define TESTDATAPATH_H

#include <QDir>
#include <QString>
#include <QtGlobal>

// Корень полевых данных для ручных сценариев. Берётся из переменной
// окружения NAV_TEST_DATA_DIR, иначе каталог "testdata" в рабочей директории.
// Раньше пути были захардкожены как C:/Users/sabrahar/...; относительные пути
// внутри корня сохранены (Desktop/FINAL/..., Documents/Ship_logs/...).
inline QString testDataPath(const QString &relative)
{
    QString root = qEnvironmentVariable("NAV_TEST_DATA_DIR");
    if (root.isEmpty())
        root = QStringLiteral("testdata");
    return QDir(root).filePath(relative);
}

#endif // TESTDATAPATH_H
