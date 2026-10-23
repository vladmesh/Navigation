#include "postprocessor.h"

#include <memory>
#include <vector>

PostProcessor::PostProcessor() {
    p190Creator = new P190_creator();
}

PostProcessor::~PostProcessor() {
    delete p190Creator;
}

int PostProcessor::mainProcess()
{

    p190Creator->setMyVault(MyVault);
    MyVault->sortItemsByCalcPriority();

    Coordinator coordinator;
    coordinator.Vault = MyVault;

    if (!ffidTimeSourceDir.isEmpty()) { //получение ffid-время из segy или .txt
        getDataFromSegy();
    } else if (!ffidTimeSourceTxtFile.isEmpty()) {
        pairs = readFileAndGeneratePairs(ffidTimeSourceTxtFile);
    } else {
        qWarning() << "no ffid source";
        return NoFfidSource;
    }
    if (pairs.size() < 2) {
        qWarning() << "not enough FFID records:" << pairs.size();
        return NotEnoughFfid;
    }

    prepareSegyData(pairs); //добавление точек для расчета, но которые в P190 писаться не будут

    QVector<int> positions(pairItemFile.size(), 0);
    std::vector<std::unique_ptr<QFile>> files;
    files.reserve(pairItemFile.size());

    for (int i = 0; i < pairItemFile.size(); i++) {
        files.push_back(std::make_unique<QFile>(pairItemFile[i].first));
    }

    int currentSuccsess = 0; //for progress bar

    for (auto pair: pairs) {
        for (int i = 0; i < pairItemFile.size(); i++) {
            QStringList parts;
            if (files[i]->fileName().toLower().endsWith(".ppk")) {
               parts = findPpkForSegy(pair, files[i].get(), &positions[i]);
            } else if (files[i]->fileName().toLower().endsWith(".nmea")) {
               parts = findNmeaForSegy(pair, files[i].get(), &positions[i]);
            } else {
                qWarning() << "unsupported navigation file" << files[i]->fileName();
                return WrongNavFile;
            }

            NmeaParser::CoordinateData first;
            NmeaParser::CoordinateData second;

            if (files[i]->fileName().toLower().endsWith(".ppk")) { //if it is
                first = PpkParser::parseLine(parts[0]);
                second = PpkParser::parseLine(parts[1]);
            } else if (files[i]->fileName().toLower().endsWith(".nmea")) {
                first = nmeaParser.parseNmeaGGA(parts[0]);
                second = nmeaParser.parseNmeaGGA(parts[1]);
            } else {
                qWarning() << "unsupported navigation file" << files[i]->fileName();
                return WrongNavFile;
            }
            NmeaParser::CoordinateData trueCoor = calcTruePosition(first, second, pair.time, first.dateTime.time(), second.dateTime.time());

            MyVault->getItem(pairItemFile[i].second)->lastGGAData = trueCoor;
        }
        for (auto i: MyVault->ItemsVault) {
            i->calcItemCoordinates();
        }
        p190Creator->createP190File(pair.julianDay);
        p190Creator->setDayOfSurvey(pair.julianDay);
        p190Creator->setFFID(pair.ffid);
        p190Creator->createShotBlock();
        currentSuccsess++;
        if (currentSuccsess % 50 == 1) {
            qDebug() << ((float)currentSuccsess/pairs.size())*100 << "% of postProcessing";
            fuckingShit->setValue(((float)currentSuccsess/pairs.size())*100);
        }
    }
    p190Creator->setHeaderIsWritten(false);
    return Ok;
}


void PostProcessor::getDataFromSegy()
{

    SegYReader sr;
    sr.readPathWithSegy(ffidTimeSourceDir);
    for (int i = 0; i < sr.times.size(); i++) {
        pairs.push_back(sr.pairs.at(i));
    }
}


QStringList PostProcessor::findNmeaForSegy(SegYReader::SegyData pair, QFile* nmeaFile, int* pos) {
    QTime time = pair.time;
    QString lowerNmea;
    QString higherNmea;
    int currentPosition = *pos;

    QStringList res;
    QString line;
    QString RMCData = "";

    if (!nmeaFile->isOpen()) {
        nmeaFile->open(QIODevice::ReadOnly | QIODevice::Text);
    }
    QTime nmeaTime;
    nmeaFile->seek(currentPosition - 100);  // Устанавливаем текущую позицию файла

    while (!nmeaFile->atEnd()) {
        line = nmeaFile->readLine().trimmed();  // Считываем строку и удаляем пробелы по краям

        if (line.mid(3, 3) == "GGA") {
            nmeaTime = nmeaParser.getTimeFromNmeaGGA(line);

            if (nmeaTime.msecsSinceStartOfDay() < time.msecsSinceStartOfDay()) {
                lowerNmea = line;  // Обновляем нижнюю NMEA строку
                currentPosition = nmeaFile->pos();  // Обновляем текущую позицию
            } else if (nmeaTime.msecsSinceStartOfDay() > time.msecsSinceStartOfDay() && higherNmea.isEmpty()) {
                higherNmea = line;  // Получаем первую строку с временем больше указанного
                break;  // Прекращаем поиск после нахождения первой строки с большим временем
            }
        } else if (line.mid(3, 3) == "RMC" || line.mid(3, 3) == "HDT") {
            RMCData = line;  // Сохраняем RMC или HDT данные
        }
    }

    // Если мы нашли верхнюю строку (higherNmea), но нижняя не была обновлена,
    // нужно вернуть предшествующую строку, которая может быть lowerNmea
    if (higherNmea.isEmpty() && !lowerNmea.isEmpty()) {
        // Перемещаемся на одну строку назад для получения GGA, если higherNmea не найден
        nmeaFile->seek(currentPosition - line.length());  // Возвращаемся на длину предыдущей строки
        if (!nmeaFile->atEnd()) {
            line = nmeaFile->readLine().trimmed();  // Считываем предыдущую строку
            if (line.mid(3, 3) == "GGA") {
                higherNmea = line;  // Устанавливаем строку как higherNmea
            }
        }
    }

    *pos = currentPosition;  // Обновляем позицию файла
    res.push_back(lowerNmea);
    res.push_back(higherNmea);
    if (!RMCData.isEmpty()) res.push_back(RMCData);


    return res;
}

QStringList PostProcessor::findPpkForSegy(SegYReader::SegyData pair, QFile *ppkFile, int* pos)
{
    QTime pairTime = pair.time;
    QString lowerPpk;
    QString higherPpk;
    int currentPosition = *pos;

    QStringList res;
    QString line;

    if (!ppkFile->isOpen()) {
        ppkFile->open(QIODevice::ReadOnly | QIODevice::Text);
    }
    QTime ppkTime;
    ppkFile->seek(currentPosition - 1);  // Устанавливаем текущую позицию файла

    while (!ppkFile->atEnd()) {
        line = ppkFile->readLine().trimmed();  // Считываем строку и удаляем пробелы по краям

        if (line.startsWith("FIN")) {
            ppkTime = PpkParser::getTimeFromPpkLine(line);
            if (pairTime.msecsSinceStartOfDay() - ppkTime.msecsSinceStartOfDay() > 600000) {
                for (int i = 0; i < 400; i++) {
                    ppkFile->readLine();
                }
            }

            if (ppkTime.msecsSinceStartOfDay() < pairTime.msecsSinceStartOfDay()) {
                lowerPpk = line;  // Обновляем нижнюю NMEA строку
                currentPosition = ppkFile->pos();  // Обновляем текущую позицию
            } else if (ppkTime.msecsSinceStartOfDay() > pairTime.msecsSinceStartOfDay() && higherPpk.isEmpty()) {
                higherPpk = line;  // Получаем первую строку с временем больше указанного
                break;  // Прекращаем поиск после нахождения первой строки с большим временем
            }
        }
    }

    // Если мы нашли верхнюю строку (higherNmea), но нижняя не была обновлена,
    // нужно вернуть предшествующую строку, которая может быть lowerNmea
    if (higherPpk.isEmpty() && !lowerPpk.isEmpty()) {
        // Перемещаемся на одну строку назад для получения GGA, если higherNmea не найден
        ppkFile->seek(currentPosition - line.length());  // Возвращаемся на длину предыдущей строки
        if (!ppkFile->atEnd()) {
            line = ppkFile->readLine().trimmed();  // Считываем предыдущую строку
        }
    }

    *pos = currentPosition;  // Обновляем позицию файла
    res.push_back(lowerPpk);
    res.push_back(higherPpk);

    return res;
}


NmeaParser::CoordinateData PostProcessor::calcTruePosition(NmeaParser::CoordinateData first,
    NmeaParser::CoordinateData second, QTime trueTime, QTime firstTime, QTime secondTime)
{
    double c1, c2;
    NmeaParser::CoordinateData res;
    if (!firstTime.isValid()) return second;
    if (!secondTime.isValid()) return first;
    if (secondTime == firstTime) return second;
    c1 = 1-((float)(trueTime.msecsSinceStartOfDay() - firstTime.msecsSinceStartOfDay())/
              (float)(secondTime.msecsSinceStartOfDay() - firstTime.msecsSinceStartOfDay()));
    c2 = 1-c1;

    res.dateTime = QDateTime(first.dateTime.date(), trueTime);
    res.height = first.height*c1 + second.height*c2;
    res.coorUTM.rx() = first.coorUTM.rx()*c1 + second.coorUTM.rx()*c2;
    res.coorUTM.ry() = first.coorUTM.ry()*c1 + second.coorUTM.ry()*c2;

    res.coordinate.setLatitude(first.coordinate.latitude()*c1 + second.coordinate.latitude()*c2);
    res.coordinate.setLongitude(first.coordinate.longitude()*c1 + second.coordinate.longitude()*c2);

    return res;
}

QVector<SegYReader::SegyData> PostProcessor::readFileAndGeneratePairs(const QString &fileName)
{
    QVector<SegYReader::SegyData> pairs;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Cannot open file!" << std::endl;
        return pairs;
    }

    QTextStream in(&file);
    // Пропускаем первую строку с заголовками
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split('\t');  // Разделитель - табуляция

        if (fields.size() == 6) {
            int ffid = fields[0].toInt();
            int julianDay = fields[1].toInt();
            int hour = fields[2].toInt();
            int minute = fields[3].toInt();
            int second = fields[4].toInt();
            int microsecond = fields[5].split('.').first().toInt();  // Отбрасываем дробную часть

            QTime time = QTime(hour, minute, second, microsecond / 1000);  // Микросекунды преобразуем в миллисекунды

            SegYReader::SegyData pair(ffid, time, julianDay);
            pairs.push_back(pair);
        }
    }
    file.close();
    return pairs;
}


QVector<SegYReader::SegyData> PostProcessor::prepareSegyData(QVector<SegYReader::SegyData> in)
{
    QVector<SegYReader::SegyData> out;
    float timeStep = 0;
    float dt;

    // Добавляем 50 записей перед первым элементом
    if (!in.isEmpty()) {
        QVector<SegYReader::SegyData>::iterator first_it = in.begin();
        dt = (std::next(first_it)->time.msecsSinceStartOfDay() - first_it->time.msecsSinceStartOfDay());

        for (int j = -50; j < 0; ++j) {
            QTime newTime = first_it->time.addMSecs(j * dt);
            if (newTime > QTime(0, 0, 0, 0)) { // Убедимся, что время не уходит в отрицательное
                SegYReader::SegyData newPair(-1, newTime, first_it->julianDay);
                out.push_back(newPair);
            }
        }
    }

    for (QVector<SegYReader::SegyData>::Iterator it = in.begin() + 1; it != in.end() - 1; ++it) {
        QVector<SegYReader::SegyData>::iterator prev_it = std::prev(it);
        QVector<SegYReader::SegyData>::iterator next_it = std::next(it);

        timeStep = it->time.msecsSinceStartOfDay() - prev_it->time.msecsSinceStartOfDay();

        if (timeStep > 2000) {
            int numToAdd = 10;
            dt = (next_it->time.msecsSinceStartOfDay() - it->time.msecsSinceStartOfDay());

            // Проверяем, чтобы добавляемое время не совпадало с prev_it
            for (int j = -numToAdd; j < 0; ++j) {
                QTime newTime = it->time.addMSecs(j * dt);
                if (newTime > prev_it->time) {
                    SegYReader::SegyData newPair(-1, newTime, it->julianDay);
                    out.push_back(newPair);
                } else {
                    // Уменьшаем количество добавляемых записей до 3
                    numToAdd = 3;
                    break;
                }
            }
        }

        out.push_back(*it);
    }

    return out;
}


void PostProcessor::setMyVault(ItemsStorage *newMyVault)
{
    MyVault = newMyVault;
}


