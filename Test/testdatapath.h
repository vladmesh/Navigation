#ifndef TESTDATAPATH_H
#define TESTDATAPATH_H

#include <QDir>
#include <QString>
#include <QtGlobal>

// Корень тестовых данных. Берётся из переменной окружения NAV_TEST_DATA_DIR,
// иначе используется каталог "testdata" рядом с рабочей директорией.
// Раньше пути были захардкожены под C:/Users/<user>/..., из-за чего тесты
// нельзя было запустить ни на другой машине, ни в CI.
inline QString testDataPath(const QString& relative)
{
    QString root = qEnvironmentVariable("NAV_TEST_DATA_DIR");
    if (root.isEmpty())
        root = QStringLiteral("testdata");
    return QDir(root).filePath(relative);
}

#endif // TESTDATAPATH_H
