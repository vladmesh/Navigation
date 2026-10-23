#include "p190headerprocessor.h"

QString P190HeaderProcessor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        content.clear();

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                content += line + '\n';
            }
        }
    }
    return content;
}

void P190HeaderProcessor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        QStringList lines = content.split('\n', Qt::SkipEmptyParts);

        for (QString &line : lines) {
            if (line.length() < 80) {
                line = line.leftJustified(80, ' ');
            }
            out << line.left(80) << '\n';
        }
    }
}

void P190HeaderProcessor::insertLine(const QString &line)
{
    if (!line.startsWith("H")) return;

    int headerCode = getHeaderCode(line);
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    int pos = 0;
    while (pos < lines.size() && headerCode >= getHeaderCode(lines[pos])) {
        ++pos;
    }
    lines.insert(pos, line);

    content = lines.join("\n") + '\n';
}

QString P190HeaderProcessor::getContent() const
{
    return content;
}

int P190HeaderProcessor::getHeaderCode(const QString &line) const
{
    bool resOk;
    int headerCode = line.mid(1, 4).toInt(&resOk);
    return resOk ? headerCode : 0;
}
