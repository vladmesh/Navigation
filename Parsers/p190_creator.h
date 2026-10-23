#ifndef P190_CREATOR_H
#define P190_CREATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRegularExpression>

#include "itemsstorage.h"
#include "functions.h"
#include "p190headeranalyzer.h"


/**
 * @class P190_creator
 * @brief Class for generating P190 survey files.
 *
 * Handles the creation of survey data blocks, header formatting, and writing
 * the data to `.p190` files based on the given input data and survey configurations.
 */
class P190_creator : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса.
     */
    explicit P190_creator(QObject *parent = nullptr);
    /**
     * @brief Деструктор класса.
     * Освобождает ресурсы, связанные с файлом вывода.
     */
    ~P190_creator();

    ///creating P190 data
    /**
     * @brief создание блока данных
     */
    QStringList createShotBlock();
    /**
     * @brief создание блока данных от косы
     * @return Список строк блока.
     */
    QStringList createStreamerBlock();
    /**
     * @brief создание блока данных от всего кроме косы
     * @return Список строк инфы о каналах
     */
    QStringList createMainInfoBlock();
    /**
     * @brief создание блока данных от всего кроме косы, но с ffid
     * @param ffid FFID
     * @return Список строк основного информационного блока.
     */
    QStringList createMainInfoBlock(int ffid);
    /**
     * @brief Создание строки для основного блока.
     * @param item Указатель на фиксированный элемент.
     * @param pointNumber Номер точки.
     * @param tailBuoy Номер хвостового буя.
     * @return Сформированная строка.
     */


    /**
     * @brief Создание строки для основного блока (новая версия).
     * @param item Указатель на фиксированный элемент.
     * @param pointNumber Номер точки.
     * @param tailBuoy Номер хвостового буя.
     * @return Сформированная строка.
     */
    QString createMainRow__new(FixedItem* item, int pointNumber, int tailBuoy);

    /// Создание файла P190 с заголовком
    void createP190File();
    /**
     * @brief Создание файла P190 для заданного дня.
     * @param dayNumber Номер дня.
     */
    void createP190File(int dayNumber);

    QStringList createHeader();
    /**
     * @brief Создание заголовка файла с указанием дня опроса.
     * @param dayNumber Номер дня.
     * @return Список строк заголовка.
     */
    QStringList createHeader(int dayNumber);


    /**
     * @brief Запись списка строк в файл.
     * @param data Список строк для записи.
     */
    void writeToFile(QStringList);
    /**
     * @brief Установка указателя на хранилище объектов.
     * @param Vault Указатель на хранилище объектов.
     */
    void setItemStoragePtr(ItemsStorage* Vault);
    QString createFileName();
    /// Установка пути для сохранения файлов
    void setPath(const QDir &newPath);
    /// Установка FFID
    void setFFID(int newFFID);
    /// Установка хранилища объектов
    void setMyVault(ItemsStorage *newMyVault);
    /// Установка дня опроса
    void setDayOfSurvey(int newDayOfSurvey);
    /// Расчет количества элементов (буев, источников)
    void calcItemsCount();
    /// Установка флага записи заголовка, чтобы не писать дважды
    void setHeaderIsWritten(bool newHeaderIsWritten);

private:
    int currentBuoyNumber; ///< Текущее номер буя.
    int currentSourceNumber; ///< Текущее номер источника.
    int currentStreamerNumber; ///< Текущее номер косы.
    p190HeaderAnalyzer headerAnalyzer;

    int FFID; ///< Идентификатор формата файла (File Format ID).

    QDateTime dummyTimeForItemsWithNoTime; ///< Время-заглушка для элементов без временных данных.
    QDateTime curDateTime; ///< Текущее время.
    int dayOfSurvey = -1; ///< День опроса.

    static QDir path; ///< Путь для сохранения файлов.
    QString fileName; ///< Имя выходного файла.
    QFile* outputFile = nullptr; ///< Указатель на файл вывода.
    ItemsStorage* MyVault = nullptr; ///< Указатель на хранилище объектов.

    QChar tailBuoy; //ну и костыль. руки бы оторвать разрабу

    QStringList initializeHeaderFile(); // Проверяем/копируем файл при запуске
    bool headerIsWritten = false; ///< Флаг, указывающий, был ли записан заголовок.

    /**
     * @brief Замена дат в строках с хэддером файла.
     * @param input Входная строка с датами.
     * @return Измененная строка с актуальными датами.
     */
    QString replaceDates(QString& input);


    ///working with HEADER and LINE
    QString lineName = "AA10AA0707077"; // len = 13


signals:

};

#endif // P190_CREATOR_H
