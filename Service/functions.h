#include <QString>
#include <QDebug>
#include <QDateTime>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

static QString floatToQString(double value, int totalLength, int decimalPlaces) {
    QString str;
    if (-1==decimalPlaces) { //костыль для номера канала
        str = QString::number(value, 'f', 0);
    } else if (1==decimalPlaces) {
        str = QString::number(value, 'f', 1);
    } else {
        str = QString::number(value, 'f', decimalPlaces);
    }
    // Если строка длиннее необходимого, обрезаем ее
    if (str.length() > totalLength) {
        str = str.left(totalLength);
    }
    // Если строка короче необходимого, добавляем пробелы в начало
    else if (str.length() < totalLength) {
        str = str.rightJustified(totalLength, ' ');
    }

    return str;
}



#endif // FUNCTIONS_H
