#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <QObject>
#include <QFileDialog>
#include <QFile>
#include <QProgressBar>

#include "DataReader/segyreader.h"
#include "coordinator.h"
#include "p190_creator.h"
// #include "logger.h"
#include "itemsstorage.h"
#include "nmeaparser.h"
#include "itemsloader.h"
#include "ppkparser.h"
#include "coordinator.h"


/**
 * @class PostProcessor
 * @brief Класс для обработки данных, полученных из файлов SEGY, NMEA и PPK и создания P190 в режиме постпроцессинга
 *
 * Этот класс отвечает за вычисление координат, синхронизацию данных из разных источников
 * (например, SEGY и NMEA), а также за создание файлов P190.
 */
class PostProcessor
{
public:
    /**
     * @brief Конструктор. Инициализирует объект.
     */
    PostProcessor();
    /**
     * @brief Основной процесс обработки данных.
     *
     * Включает чтение данных, расчет координат и создание файлов P190.
     * @return Код результата выполнения.
     */
    int mainProcess();


    /**
     * @brief Чтение данных из SEGY.
     */
    void getDataFromSegy();
    /**
     * @brief Устанавливает хранилище объектов.
     *
     * @param newMyVault Указатель на объект ItemsStorage.
     */
    void setMyVault(ItemsStorage *newMyVault);
    
    QDir nmeaStorage; ///< Директория хранения файлов NMEA.
    QDir segyStorage; ///< Директория хранения файлов SEGY.
    /**
     * @brief Устанавливает путь для хранения файлов NMEA.
     *
     * @param newNmeaStorage Новый путь.
     */
    void setNmeaStorage(const QDir &newNmeaStorage);
    /**
     * @brief Устанавливает путь для хранения файлов SEGY.
     *
     * @param newSegyStorage Новый путь.
     */
    void setSegyStorage(const QDir &newSegyStorage);


    /**
     * @brief Заполняет векторы объектов с подключениями и без подключений.
     */
    void fillItemsVectors();
    int findNmeaFiles();


    /**
     * @brief Устанавливает P190_creator.
     *
     * @param newP190 Указатель на объект P190_creator.
     */
    void setP190(P190_creator *newP190);
    QVector<SegYReader::SegyData> pairs;
    /**
     * @brief Подготовка данных SEGY, например, добавление интерполированных значений.
     *
     * @param in Входные данные SEGY.
     * @return Подготовленные данные SEGY.
     */
    QVector<SegYReader::SegyData> prepareSegyData(QVector<SegYReader::SegyData> in);

    /**
     * @brief Поиск соответствующих данных NMEA для записи SEGY.
     * Пока поиск почти линейный, надо сделать параметризированным
     * @param pair Данные SEGY.
     * @param nmeaFile Файл NMEA.
     * @param pos Текущая позиция в файле.
     * @return Список строк из файла NMEA.
     */
    QStringList findNmeaForSegy(SegYReader::SegyData pair, QFile* nmeaFile, int* pos);
    /**
     * @brief Поиск соответствующих данных PPK для записи SEGY.
     * Пока поиск почти линейный, надо сделать параметризированным
     * @param pair Данные SEGY.
     * @param ppkFile Файл PPK.
     * @param pos Текущая позиция в файле.
     * @return Список строк из файла PPK.
     */
    QStringList findPpkForSegy(SegYReader::SegyData pair, QFile* ppkFilem, int* pos);
    /**
     * @brief Вычисление истинной позиции по двум точкам
     *
     * @param first Первая координата.
     * @param second Вторая координата.
     * @param trueTime Истинное время.
     * @param firstTime Время первой координаты.
     * @param secondTime Время второй координаты.
     * @return Истинная позиция.
     */
    NmeaParser::CoordinateData calcTruePosition(NmeaParser::CoordinateData first,
        NmeaParser::CoordinateData second, QTime trueTime, QTime firstTime, QTime secondTime);

    /**
     * @brief Чтение файла и генерация информации для постпроцессинга.
     *
     * @param fileName Имя файла SEGY или текстового файла.
     * @return Вектор информации для постпроцессинга.
     */
    QVector<SegYReader::SegyData> readFileAndGeneratePairs(const QString& fileName);


    QVector<QPointer<FixedItem>> vectorWithCon; ///< Вектор объектов с подключениями.
    QVector<QPointer<FixedItem>> vectorNoCon; ///< Вектор объектов без подключений.

    ItemsStorage* MyVault;///< Указатель на хранилище объектов.
    NmeaParser nmeaParser;///< Объект парсера NMEA. По логике не нужен, но почему-то нужен
    P190_creator* p190Creator;///< Указатель на объект P190_creator.


//взаимодействие с интерфейсом PostProcessorView
    // ItemsStorage Vault;
    QVector<QPointer<FixedItem>> items; ///< Вектор указателей на объекты.
    int itemsWithConnection = 0;

    QFile* jsonSchemeFile;
    // QVector<QFile*> nmeaFiles;
    QVector<QFile*> NmeaFiles;///< Вектор файлов NMEA для поспроцессинга.
    QVector<QFile*> ppkFiles;///< Вектор файлов PPK для поспроцессинга..
    QString ffidTimeSourceTxtFile; ///< Имя текстового файла с источником времени - FFID.
    QString ffidTimeSourceDir; ///< Директория с файлами SEGY.

    float percentageP190Creation = 0; ///< Процент выполнения создания P190.
    QVector<QPair<QString, QString>> pairItemFile; ///< Вектор пар "имя объекта - имя файла".

    QProgressBar* fuckingShit;

};

#endif // POSTPROCESSOR_H
