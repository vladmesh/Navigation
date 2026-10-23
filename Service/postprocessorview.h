#ifndef POSTPROCESSORVIEW_H
#define POSTPROCESSORVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QRadioButton>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>

#include "postprocessor.h"
/**
 * @class PostProcessorView
 * @brief Представление для управления постпроцессингом.
 *
 * Этот класс предоставляет графический интерфейс для настройки параметров и запуска
 * обработки данных, используя объект класса `PostProcessor`.
 */
class PostProcessorView : public QWidget
{
    Q_OBJECT
    explicit PostProcessorView(QWidget *parent = nullptr);

public:
    /**
     * @brief Удаляет возможность копирования объекта.
     */
    PostProcessorView& operator=(const PostProcessorView&) = delete;

    /**
     * @brief Метод для получения единственного экземпляра PostProcessorView.
     *
     * Использует паттерн "Одиночка" (Singleton).
     * @return Указатель на единственный экземпляр PostProcessorView.
     */
    static PostProcessorView* getInstance();

    /**
     * @brief Инициализация пользовательского интерфейса.
     */
    void setupUI();
private:
    //выбранные юзверем в pprv значения для передачи в ppr
    QVector<QString> nmeaNames; ///< Пути к NMEA файлам.
    QVector<QString> ppkNames; ///< Пути к PPK файлам.
    QString ffidTimeSourceTxt = ""; ///< Текстовый файл с источником FFID.
    QString ffidTimeSourceDir = ""; ///< Директория SEGY файлов.
    QVector<QPair<QString, QString>> pairItemFile; ///< Пары: название объекта и файл данных.

    QPushButton* chooseSchemeButton = nullptr;
    QLineEdit* schemeLineEdit = nullptr;
    QPushButton* runButton = nullptr;
    QProgressBar* progressBar = nullptr;
    QVector<QPushButton*> nmeaPpkButtons;
    QVector<QLineEdit*> nmeaPpkLineEdits;
    QPushButton* chooseFfidButton = nullptr;
    QLineEdit* ffidLineEdit = nullptr;
    QRadioButton* segyRadioButton = nullptr;
    QRadioButton* txtRadioButton = nullptr;
    QVBoxLayout* mainLayout = nullptr;


    PostProcessor postProc; ///< Экземпляр класса PostProcessor, выполняющий обработку данных.


    /**
     * @brief Передача выбранных параметров из интерфейса в PostProcessor.
     */
    void sendValuesToPostProc();
signals:

private slots:
    /**
     * @brief Метод для выбора схемы (JSON файл).
     */
    void chooseScheme();

    /**
     * @brief Метод для создания кнопок и полей для ввода путей к файлам NMEA/PPK.
     */
    void createConnectionButtons();
    /**
     * @brief Метод для выбора источника FFID (директория SEGY или файл TXT).
     */
    void chooseFfidSource();
    /**
     * @brief Проверка, заполнены ли все обязательные поля интерфейса.
     *
     * Если все поля заполнены, кнопка "RUN" активируется.
     */
    void checkReadyToRun();
    /**
     * @brief Действия, выполняемые при нажатии на кнопку "RUN".
     */
    void onRunButtonClicked();

    void dropEvent(QDropEvent *event) override;
};

#endif // POSTPROCESSORVIEW_H
