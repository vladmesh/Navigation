#ifndef STREAMERDIALOG_H
#define STREAMERDIALOG_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QMessageBox>


// #include "itemsstorage.h"

/**
 * @class StreamerDialog
 * @brief Класс диалогового окна для настройки параметров косы.
 *
 * Этот класс предоставляет интерфейс для выбора и настройки параметров косы,
 * таких как количество каналов и расстояние между ними.
 * Импорт/экспорт CSV перенесён в archive/snippets/streamerdialog-csv.cpp.
 */
class StreamerDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор, инициализирует диалоговое окно для настройки стримера.
     * @param parent Указатель на родительский виджет.
     */
    StreamerDialog(QWidget *parent = nullptr);
    // StreamerDialog(ItemsStorage* Vault);

    QString retrunValue; ///< Возвращаемое значение диалога.
    uint numOfChannels; ///< Количество каналов стримера.
    QVector<double> chans; ///< Вектор расстояний между каналами.
    double elasticSectionLength; ///< не используется
    double totalLength; ///< Общая длина стримера.
    double leadingDamperLength; ///< не используется
    double endDamperLength; ///<не используется
    QString dChString; ///< Строка с информацией о расстоянии между каналами.
    QString singleChannelType; ///< Тип одиночного канала стримера.

private:
    QPushButton *finishButton = nullptr; ///< Кнопка завершения настройки стримера.
    QTabWidget *tabWidget = nullptr; ///< Вкладки для выбора режима настройки (одно/много - канальная коса).
    QWidget *singleChannelTab = nullptr; ///< Вкладка для настройки одноканальной косы
    QWidget *multiChannelTab = nullptr; ///< Вкладка для настройки многоканальной косы.

    QComboBox *singleChannelComboBox = nullptr; ///< Выпадающий список для выбора типа одноканальной косы
    QComboBox *multiChannelComboBoxChannels = nullptr; ///< Выпадающий список для выбора количества каналов.
    QComboBox *multiChannelComboBoxDistance = nullptr; ///< Выпадающий список для выбора расстояния между каналами.
    /**
     * @brief Создает значения расстояний между каналами.
     */
    void createChansValues();
    QLabel* statusLabel;

private slots:
    /**
     * @brief Слот для завершения настройки и сохранения значений.
     */
    void onFinishButtonClicked();
    void updateStatusLabel();
};

#endif // STREAMERDIALOG_H
