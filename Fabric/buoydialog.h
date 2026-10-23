#ifndef BUOYDIALOG_H
#define BUOYDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMessageBox>

#include "itemsstorage.h"
#include "streamer.h"

/**
 * @class BuoyDialog
 * @brief Класс диалогового окна для настройки параметров буя.
 *
 * Диалоговое окно позволяет пользователю задавать параметры высоты антенны и глубины буксировки.
 * Если объект требует соединения, предоставляется возможность выбора связанного объекта Streamer.
 */
class BuoyDialog : public QDialog
{
    Q_OBJECT
public:

    /**
     * @brief Конструктор, инициализирует диалоговое окно для настройки параметров буя.
     * @param vault Указатель на хранилище объектов ItemsStorage.
     * @param needConnection Флаг, указывающий, требуется ли подключение к объекту Streamer.
     */
    BuoyDialog(ItemsStorage* vault, bool needConnection);
    double AnthenaHeight; ///< Высота антенны.
    double TowingDepth; ///< Глубина буксировки.

protected:
    ItemsStorage* MyVault; ///< Хранилище объектов для выбора.
    QComboBox *comboBox; ///< Выпадающий список для выбора объекта Streamer.
    QDoubleSpinBox *spinBox1; ///< SpinBox для установки высоты антенны.
    QDoubleSpinBox *spinBox2; ///< SpinBox для установки глубины буксировки.
    QLabel *spinBoxLabel1;
    QLabel *spinBoxLabel2;
private:
    QString selectedStreamerName = ""; ///< Имя выбранного объекта Streamer.
    bool needConnection; ///< Флаг, указывающий, требуется ли подключение к Streamer.
    Streamer* selectedStreamer = nullptr; ///< Указатель на выбранный объект Streamer


private slots:
    /**
     * @brief Слот, вызываемый при нажатии кнопки "Set".
     *
     * Сохраняет выбранный объект Streamer (если требуется подключение),
     * а также значения высоты антенны и глубины буксировки.
     */
    void onSetButtonClicked();
};

#endif // BUOYDIALOG_H
