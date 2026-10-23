#include "segyreader.h"

QDate getDateFromDayOfYear(int dayOfYear) {
    // Проверяем, что номер дня находится в допустимом диапазоне
    if (dayOfYear < 1 || dayOfYear > 366) {
        // qWarning() << "Номер дня должен быть от 1 до 366.";
        return QDate(); // Возвращаем некорректную дату
    }
    int year = QDate::currentDate().year();
    // Получаем первую дату года
    QDate firstDayOfYear(year, 1, 1);

    // Возвращаем дату, прибавляя номер дня минус один
    return firstDayOfYear.addDays(dayOfYear - 1);
}


SegYReader::SegYReader() {}

SegYReader::SegYReader(const QString& filePath) : filePath(filePath) {}

bool SegYReader::readFile() {
    QFile file(filePath);
    if (QFileInfo(file).suffix() != "sgy") {
        // qDebug() << file.fileName() << " not segy";
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Could not open file: " << filePath.toStdString() << std::endl;
        return false;
    }

    file.seek(3600); // Skip the textual and binary file headers (3600 bytes)

    readTraceHeader(file);

    return true;
}

void SegYReader::readPathWithSegy(QDir path)
{
    for (QString i: path.entryList()) {
        // qDebug() << i;

        setFilePath(path.path()+"/"+i);
        readFile();
    }
}

void SegYReader::setFilePath(const QString &newFilePath)
{
    filePath = newFilePath;
}

void SegYReader::readTraceHeader(QFile& file) {
    //
    //
    //ATTENTION. here is is 1 byte shift for each header. data[158] is 159 byte
    //it's obvious, but better to watch out if reading segy
    //


    //Assuming each trace header is 240 bytes long
    QByteArray traceHeaderData = file.read(240);

    if (traceHeaderData.size() < 240) {
        return; // Not enough data
    }

    // Use QByteArray to interpret the bytes
    const uchar* data = reinterpret_cast<const uchar*>(traceHeaderData.data());

    // Extracting FFID from bytes 9-12 (4 bytes, UI4)
    // uint32_t ffid = (data[12] << 24) | (data[13] << 16) | (data[14] << 8) | data[15];
    uint32_t ffid = data[9] << 16 | data[10] << 8 | data[11];



    uint16_t JulianDay = data[158]<<8 | data[159];
    uint16_t hours = data[161];
    uint16_t minutes = data[163];
    uint16_t seconds = data[165];

    uint32_t microseconds = (data[233] << 16) | (data[234]) << 8 | data[235];
    ffids.push_back(ffid);
    times.push_back(QTime(hours, minutes, seconds, microseconds/1000));
    pairs.push_back(SegyData(ffid,  QTime(hours, minutes, seconds, microseconds/1000), JulianDay));

    file.close();
}


