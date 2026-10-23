#ifndef CONNECTION_CREATOR_H
#define CONNECTION_CREATOR_H

#include <QDialog>
#include <QVector>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSerialPortInfo>
#include <QPointer>

#include "connection_com.h"
#include "connection_net.h"
// #include "connection_udp.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Connection_creator;
}
QT_END_NAMESPACE

/**
 * @class Connection_creator
 * @brief Класс для создания различных типов соединений.
 *
 * Этот класс предоставляет интерфейс для выбора и создания сетевого (TCP/UDP) или COM-соединения,
 * а также позволяет сохранять информацию о последнем созданном соединении.
 * Класс вызывается при создании объектов с ГНСС и имеет интерфейс
 * По сути не фабрика, но чисто интерфейс для ввода значений.
 * Хранит в себе указатели на все созданные соединения
 */
class Connection_creator : public QDialog
{
    Q_OBJECT

public:

    Connection_creator(QWidget *parent = nullptr);
    ~Connection_creator();
    /**
     * @brief Создает новое соединение на основе выбора в интерфейсе.
     *
     * Создает соединение типа TCP, UDP или COM, используя параметры,
     * указанные пользователем, и добавляет его в список созданных соединений.
     * @return Указатель на созданное соединение Connection.
     */
    Connection* createConnection();
    Connection* createConnection(QHostAddress IP, int port, QString fileName);
    Connection* createConnection(QString COMport, int baudRate, QString fileName);

    /**
     * @brief Получает последнее созданное соединение
     * нужен потому что не умею получать значение из QDialog->exec
     * @return Указатель на последнее созданное соединение.
     */
    Connection* getLastConnection();

    int exec() override;

    void setDirPath(const QDir &newDirPath);


    /**
     * @brief Устанавливает имя последнего созданного файла.
     * похожая причина что и в getLastConnection()
     * @param newLastCreatedFileName Имя файла.
     */
    void setLastCreatedFileName(const QString &newLastCreatedFileName);
    void updateCOMPortsList();
private slots:

    /**
     * @brief Слот для завершения диалога и создания соединения.
     * Вызывает создание нового соединения и отображает его.
     */
    void on_pushButton_clicked();

    void on_connection_types_currentChanged(int index);

signals:
    /**
     * @brief Сигнал, испускаемый после создания нового соединения.
     */
    void connectionCreated();

private:
    Connection* lastConnection; ///< Указатель на последнее созданное соединение.
    QDir dirPath; ///< Путь к директории для сохранения файлов.

    QString lastCreatedFileName; ///< Имя последнего созданного файла.

    Ui::Connection_creator *ui; ///< Интерфейс пользователя для класса Connection_creator.
    QVector<QPointer<Connection>> Connection_vector; ///< Вектор созданных соединений.

    /**
     * @brief Скрывает опцию для создания UDP-соединения.
     * Нужна потому что есть прототип UDP, но он не отлажен и не работает
     * Убирает элементы интерфейса, связанные с выбором UDP.
     */
    void hideUDPPossibility();
};

#endif // CONNECTION_CREATOR_H
