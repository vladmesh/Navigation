#ifndef PPKPARSER_H
#define PPKPARSER_H

#include <QObject>
#include <QTextStream>
#include <QRegularExpression>
#include <QString>
#include <QRegularExpression>

#include "nmeaparser.h"
/// @brief Класс PpkParser предназначен для обработки данных PPK (постобработанных GPS данных).
/// Наследуется от NmeaParser, чтобы использовать существующую функциональность и структуры.
/// Класс включает методы для преобразования координат, сравнения данных, парсинга строк PPK,
/// а также получения времени из строк формата PPK.
class PpkParser : public NmeaParser
{
    Q_OBJECT
public:
    /// @brief Конструктор класса PpkParser.
    /// @param parent Указатель на родительский объект.
    PpkParser(QObject *parent = nullptr);

    /// @brief Преобразует координаты из формата "градусы, минуты, секунды" в десятичные градусы.
    /// @param degrees Градусы.
    /// @param minutes Минуты.
    /// @param seconds Секунды.
    /// @return Значение в десятичных градусах.
    static double dmsToDecimal(int degrees, int minutes, double seconds);
    /// @brief Разбирает строку PPK-файла и возвращает координатные данные.
    /// @param line Строка из PPK-файла.
    /// @return Структура CoordinateData с разобранными данными.
    static CoordinateData parseLine(const QString &line);
    /// @brief Сравнивает координаты из NMEA и PPK.
    /// @param nmea Координаты из NMEA.
    /// @param ppk Координаты из PPK.
    /// @return true, если координаты совпадают, иначе false.
    static bool compareCoordinates(CoordinateData nmea, CoordinateData ppk);
    /// @brief Извлекает время из строки PPK.
    /// @param PpkSentence Строка из PPK-файла.
    /// @return Время в формате QTime.
    static QTime getTimeFromPpkLine(const QString &PpkSentence);

signals:
};

#endif // PPKPARSER_H
