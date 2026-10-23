#ifndef CONNECTION_COM_H
#define CONNECTION_COM_H

// #include "connection.h"
#include "connectionlogic.h"

#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class Connection_com;
}
/**
 * @class Connection_com
 * @brief Класс для обработки соединения через последовательный порт (COM-порт).
 *
 * Наследует функционал класса Connection и добавляет поддержку COM-порта.
 * Позволяет задавать параметры порта, такие как скорость передачи данных и имя COM-порта,
 * а также управлять подключением и чтением данных.
 */
class Connection_com : public Connection
{
    Q_OBJECT

public:
    explicit Connection_com();
    /**
     * @brief Конструктор с параметрами.
     * @param COM_port Имя COM-порта для подключения.
     * @param BaudRate Скорость передачи данных.
     * @param filename Имя файла для записи данных в формате NMEA.
     */
    Connection_com(QString COM_port, int BaudRate, QString filename);

    ~Connection_com() override;
    // void recieve_data(QByteArray data);

    QString filename;
    QString getComPort();

    int getBaudRate() const;
    /**
     * @brief Выполняет переподключение к COM-порту с использованием сохранённых параметров.
     */
    void reconnect() override;

private:
    // Ui::Connection_com *ui; ///< Интерфейс пользователя для класса Connection_com.
    QSerialPort SerialPort; ///< Объект для работы с COM-портом.

    QByteArray data; ///< Буфер данных для чтения из порта.
    QString COM_port; ///< Имя COM-порта.
    int BaudRate = 19200; ///< Скорость передачи данных, по умолчанию 19200.



private slots:
    /**
     * @brief Слот для обработки поступающих данных из COM-порта.
     * Читает данные из порта, проверяет корректность и обрабатывает их.
     */
    void ReadyRead() override;
};

#endif // CONNECTION_COM_H
