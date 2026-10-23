#ifndef SEGYREADER_H
#define SEGYREADER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QVector>
#include <QString>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QTime>
#include "functions.h"

/**
 * @class SegYReader
 * @brief Класс для чтения и обработки файлов формата SEG-Y.
 *
 * Этот класс позволяет считывать заголовки трасс из файлов SEG-Y и извлекать информацию
 * о FFID, времени и юлианском дне из каждого заголовка. Данные сохраняются в соответствующих
 * типах данных для дальнейшей обработки.
 */
class Q_DECL_EXPORT SegYReader
{
public:
    /**
     * @struct SegyData
     * @brief Структура для хранения информации о FFID, времени и юлианском дне.
     * Структура хранит идентификатор FFID, время и юлианский день, извлеченные
     * из заголовка трассы SEG-Y.
     */
    struct SegyData{
        int ffid;
        QTime time;
        int julianDay;
        SegyData(int ffid, QTime time, int julianDay) :ffid(ffid), time(time), julianDay(julianDay){};
    };

    QVector<int> ffids; ///< Вектор идентификаторов FFID.
    QVector<QTime> times; ///< Вектор временных меток.
    QVector<SegyData> pairs; ///< Вектор структур SegyData, содержащих данные из заголовка трассы.

    SegYReader();
    /**
     * @brief Конструктор с указанием пути к файлу.
     * @param filePath Путь к файлу SEG-Y.
     */
    SegYReader(const QString& filePath);

    /**
     * @brief Читает файл SEG-Y и извлекает данные.
     *
     * Проверяет формат файла, открывает его для чтения и начинает обработку данных.
     * @return true, если файл успешно прочитан; false, если возникли ошибки.
     */
    bool readFile();

    /**
     * @brief Читает все файлы формата SEG-Y в указанной директории.
     * @param path Директория с файлами SEG-Y.
     */
    void readPathWithSegy(QDir path);

    QString filePath;///< Путь к текущему файлу SEG-Y.

    /**
     * @brief Устанавливает путь к файлу для чтения.
     * @param newFilePath Новый путь к файлу.
     */
    void readTraceHeader(QFile& file);
    /**
     * @brief Устанавливает путь к файлу для чтения.
     * @param newFilePath Новый путь к файлу.
     */
    void setFilePath(const QString &newFilePath);


};

#endif // SEGYREADER_H
