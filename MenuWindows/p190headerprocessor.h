#ifndef P190HEADERPROCESSOR_H
#define P190HEADERPROCESSOR_H

#include <QFile>
#include <QStringList>
#include <QTextStream>


class Q_DECL_EXPORT P190HeaderProcessor
{
public:
    QString loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void insertLine(const QString &line);

    QString content; //хранилище всей информации для заголовка P190
private:
    int getHeaderCode(const QString &line) const;
};

#endif // P190HEADERPROCESSOR_H
