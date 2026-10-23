#ifndef CONNECTION_NET_H
#define CONNECTION_NET_H

// #include "connection.h"
#include "connectionlogic.h"
#include <QTcpSocket>

namespace Ui {
class Connection_Net;
}
/**
 * @class Connection_Net
 * @brief Класс для управления сетевым соединением через TCP.
 *
 * Наследует функционал класса Connection и добавляет поддержку соединения через TCP/IP.
 * Позволяет подключаться к заданному IP-адресу и порту, а также управлять
 * передачей данных и обработкой сетевых событий.
 */
class Connection_Net : public Connection
{
    Q_OBJECT

public:
    explicit Connection_Net();
    /**
     * @brief Конструктор с параметрами.
     * @param IP IP-адрес для подключения.
     * @param port Порт для подключения.
     * @param filename Имя файла для записи данных в формате NMEA.
     */
    Connection_Net(QString IP, int port, QString filename);

    ~Connection_Net() override;
    // void recieve_data(QByteArray data);


    QString filename;
    QString getIP_port();

    int getPort() const;
    /**
     * @brief Выполняет переподключение к сети с использованием сохранённых параметров.
     */
    void reconnect() override;
    QString getIP() const;

private:
    QByteArray data; ///< Буфер данных для чтения из сети.
    QTcpSocket* socket; ///< Сокет для управления TCP-соединением.
    QString IP; ///< IP-адрес для подключения.
    int port; ///< Порт для подключения.
    // Ui::Connection_Net *ui; ///< Интерфейс пользователя для класса Connection_Net.

private slots:
    /**
     * @brief Слот для обработки поступающих данных из сети.
     * Читает данные из сокета, проверяет корректность и обрабатывает их.
     */
    void ReadyRead() override;

    /**
     * @brief Слот, вызываемый при успешном подключении.
     */
    void onConnected();

    /**
     * @brief Слот, вызываемый при отключении соединения.
     */
    void onDisconnected();

    /**
     * @brief Слот для обработки ошибок сокета.
     * @param socketError Код ошибки сокета.
     */
    void onError(QAbstractSocket::SocketError socketError);
};

#endif // CONNECTION_NET_H
