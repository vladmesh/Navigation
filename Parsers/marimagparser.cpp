#include "marimagparser.h"

MariMagParser::MariMagParser() {}

void MariMagParser::parseMariMagFile(QString fileName)
{
    // QFile inFile = QFile(fileName);
    // if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     qWarning() << __FUNCTION__ <<"file not open";
    //     return;
    // }
    // QFile outFile = QFile(addSuffixToFilename(fileName, "V1"));
    // outFile.open(QIODevice::WriteOnly | QIODevice::Text);

    // QMap<QTime, MagData> myMap;
    // QTextStream stream(&inFile);

    // QString inLine;

    // CommonMagData comData;
    // MagData magData;
    // QTime curTime;
    // FullData dataToCreateLine;

    // while (!stream.atEnd()){
    //     inLine = stream.readLine();
    //     comData = getCommonMagData(inLine);
    //     magData = getMagData(inLine);
    //     curTime = comData.dateTime.time();
    //     if (myMap.contains(curTime)) {
    //         dataToCreateLine.commonData = comData;
    //         dataToCreateLine.first = myMap[curTime];
    //         dataToCreateLine.second = magData;
    //         outFile.write((createLine(dataToCreateLine) + "\n").toStdString().data());
    //         myMap.erase(myMap.find(curTime));
    //     } else {
    //         myMap.insert(curTime, magData);
    //     }
    // }
}

MariMagParser::CommonMagData MariMagParser::getCommonMagDataV1(QString inLine)
{

    QStringList separetedData = inLine.split(";");
    qDebug() << separetedData<< __FUNCTION__;
    CommonMagData res;
    return res;
}

MariMagParser::CommonMagData MariMagParser::getCommonMagData(QString inLine)
{
        QStringList separetedData = inLine.split(";");
        qDebug() << separetedData<< __FUNCTION__;
        CommonMagData res;

        res.dateTime = QDateTime::fromString(separetedData[0] + ' ' + separetedData[1], "dd.MM.yy HH:mm:ss.zzz");
        res.lat = separetedData[2].toDouble();
        res.lon = separetedData[3].toDouble();
        res.latc = separetedData[18].toDouble();
        res.lonc = separetedData[19].toDouble();
        res.altc = separetedData[20].toDouble();
        res.layback = separetedData[16].toDouble();
        res.baseline = separetedData[17].toDouble();
        return res;
}



// MariMagParser::CommonMagData MariMagParser::getCommonMagData(QString inLine)
// {
//     QStringList separetedData = inLine.split(";");
//     qDebug() << separetedData<< __FUNCTION__;


//     CommonMagData res;
//     res.dateTime = QDateTime::fromString(separetedData[1], "dd.MM.yy HH:mm:ss.zzz");
//     res.lat = separetedData[3].toDouble();
//     res.lon = separetedData[4].toDouble();
//     res.latc = separetedData[5].toDouble();
//     res.lonc = separetedData[6].toDouble();
//     res.altc = separetedData[13].toDouble();
//     res.layback = separetedData[18].toDouble();
//     res.baseline = 1.5; ///hat should be here???
//     return res;
// }

MariMagParser::MagData MariMagParser::getMagDataV1(QString inLine)
{
    QStringList separetedData = inLine.split(";");
    qDebug() << separetedData << __FUNCTION__;
    MagData res;
    res.fishID = separetedData[0].toInt();
    res.field = separetedData[7].toFloat();
    res.error = separetedData[8].toFloat();
    res.depth = separetedData[12].toFloat();
    res.alt = separetedData[13].toFloat();
    res.pitch = separetedData[10].toInt();
    res.roll = separetedData[11].toInt();
    return res;
}

MariMagParser::FullData MariMagParser::getFullData(std::string inLine)
{
    MariMagParser::FullData res;
    return res;
}

MariMagParser::FullData MariMagParser::getFullData(QString inLine)
{
    MariMagParser::FullData res;
    QStringList words = inLine.split(";");
    res.commonData.dateTime = QDateTime::fromString(words[0] + ' ' + words[1], "dd.MM.yy HH:mm:ss.zzz");
    res.commonData.lat = words[2].toDouble();
    res.commonData.lon = words[3].toDouble();
    res.first.fishID = 0;
    res.first.field = words[4].toDouble();
    res.first.error = words[5].toDouble();
    res.first.depth = words[6].toDouble();
    res.first.alt = words[7].toDouble();
    res.first.pitch = words[8].toDouble();
    res.first.roll = words[9].toDouble();
    res.second.fishID = 1;
    res.second.field = words[6+4].toDouble();
    res.second.error = words[6+5].toDouble();
    res.second.depth = words[6+6].toDouble();
    res.second.alt = words[6+7].toDouble();
    res.second.pitch = words[6+8].toDouble();
    res.second.roll = words[6+9].toDouble();
    res.commonData.layback = words[16].toDouble();
    res.commonData.baseline = words[17].toDouble();
    res.commonData.latc = words[18].toDouble();
    res.commonData.lonc = words[19].toDouble();
    res.commonData.altc = words[20].toDouble();
    return res;
}




QString MariMagParser::createLine(QStringList in) // переделать под структурки
{
    QString res = "";

    MariMagParser::MagData first = getMagDataV1(in[0]);
    MariMagParser::MagData second = getMagDataV1(in[1]);
    if (first.fishID == 1) {
        MariMagParser::MagData tmp = first;
        first = second;
        second = tmp;
    }

    MariMagParser::CommonMagData firstCommon = getCommonMagDataV1(in[0]);
    MariMagParser::CommonMagData secondCommon = getCommonMagDataV1(in[1]);
    if (!(firstCommon == secondCommon)) { //если попали разные строки, то выходим
        qWarning() <<__FUNCTION__<<"different common data";
        return QString();
    }

    res += firstCommon.dateTime.date().toString() + ';';
    res += firstCommon.dateTime.time().toString() + ';';
    res += QString::number(firstCommon.lat)+ ';';
    res += QString::number(firstCommon.lon)+ ';';
    res += QString::number(first.field) + ';';
    res += QString::number(first.error) + ';';
    res += QString::number(first.depth) + ';';
    res += QString::number(first.alt) + ';';
    res += QString::number(first.pitch) + ';';
    res += QString::number(first.roll) + ';';
    res += QString::number(second.field) + ';';
    res += QString::number(second.error) + ';';
    res += QString::number(second.depth) + ';';
    res += QString::number(second.alt) + ';';
    res += QString::number(second.pitch) + ';';
    res += QString::number(second.roll) + ';';
    res += QString::number(firstCommon.layback)+ ';';
    res += QString::number(firstCommon.baseline)+ ';';
    res += QString::number(firstCommon.latc)+ ';';
    res += QString::number(firstCommon.lonc)+ ';';
    res += QString::number(firstCommon.altc)+ ';';

    return res;
}

QString MariMagParser::createLine(FullData in)
{
    QString res = "";
    res += in.commonData.dateTime.date().toString() + ';';
    res += in.commonData.dateTime.time().toString() + ';';
    res += QString::number(in.commonData.lat)+ ';';
    res += QString::number(in.commonData.lon)+ ';';
    res += QString::number(in.first.field) + ';';
    res += QString::number(in.first.error) + ';';
    res += QString::number(in.first.depth) + ';';
    res += QString::number(in.first.alt) + ';';
    res += QString::number(in.first.pitch) + ';';
    res += QString::number(in.first.roll) + ';';
    res += QString::number(in.second.field) + ';';
    res += QString::number(in.second.error) + ';';
    res += QString::number(in.second.depth) + ';';
    res += QString::number(in.second.alt) + ';';
    res += QString::number(in.second.pitch) + ';';
    res += QString::number(in.second.roll) + ';';
    res += QString::number(in.commonData.layback)+ ';';
    res += QString::number(in.commonData.baseline)+ ';';
    res += QString::number(in.commonData.latc)+ ';';
    res += QString::number(in.commonData.lonc)+ ';';
    res += QString::number(in.commonData.altc)+ ';';
    return res;
}

QString MariMagParser::addSuffixToFilename(const QString &filePath, const QString &suffix)
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.completeBaseName(); // Имя файла без расширения
    QString extension = fileInfo.suffix();         // Расширение файла
    QString dir = fileInfo.absolutePath();         // Путь к директории
    return QString("%1/%2%3.%4").arg(dir).arg(baseName).arg(suffix).arg(extension);
}
