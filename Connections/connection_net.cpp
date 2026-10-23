#include "connection_net.h"

Connection_Net::Connection_Net()
{
}

/**
 * @brief Конструктор с параметрами для инициализации сетевого соединения и файла записи.
 * @param IP IP-адрес для подключения.
 * @param port Порт для подключения.
 * @param filename Имя файла для записи данных.
 *
 * Устанавливает параметры TCP-соединения, инициализирует файл записи и
 * подключает сигналы сокета к слотам для обработки событий.
 */
Connection_Net::Connection_Net(QString IP, int port, QString filename) :
    filename(filename), IP(IP), port(port)
{

    ui->setAttribute(Qt::WA_DeleteOnClose);

    create_file_for_nmea(this->filename); //создание файла
    this->lastRecievedGGA = "empty net";


    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Connection_Net::onConnected);
    connect(socket,
            &QTcpSocket::readyRead, this, &Connection_Net::ReadyRead);
    connect(socket, &QTcpSocket::
                    disconnected, this, &Connection_Net::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::
            of(&QAbstractSocket::errorOccurred), this, &Connection_Net::onError);
    ///конектим сигналы сокета о событиях с слотами нашего объекта соединения


    socket->connectToHost(QHostAddress(IP), port);
    ui->setWindowTitle(IP + " :" + QString::number(port));
}


Connection_Net::~Connection_Net()
{
    file->close();
    socket->close();
    // if (ui) delete ui;
    delete socket;
}

QString Connection_Net::getIP_port()
{
    return IP + " " + QString::number(port);
}

int Connection_Net::getPort() const
{
    return port;
}
/**
 * @brief Выполняет переподключение к указанному IP-адресу и порту.
 *
 * Закрывает текущее подключение, удаляет сокет и создает новое соединение с параметрами,
 * сохраненными в полях IP и port.
 */
void Connection_Net::reconnect()
{
    socket->close();
    delete socket;
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Connection_Net::onConnected);
    connect(socket,
            &QTcpSocket::readyRead, this, &Connection_Net::ReadyRead);
    connect(socket, &QTcpSocket::
                    disconnected, this, &Connection_Net::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::
            of(&QAbstractSocket::errorOccurred), this, &Connection_Net::onError);
    ///конектим сигналы сокета о событиях с слотами нашего объекта соединения


    socket->connectToHost(QHostAddress(IP), port);
}

QString Connection_Net::getIP() const
{
    return IP;
}

/**
 * @brief Слот, вызываемый при успешном подключении к серверу.
 *
 * Выводит сообщение об успешном подключении с указанием адреса и порта.
 */
void Connection_Net::onConnected()
{
    qDebug() << "Connected to" << socket->peerAddress().toString() << ":" << socket->peerPort();
}


/**
 * @brief Слот для обработки поступающих данных из сети.
 *
 * Читает данные из сокета, проверяет их корректность и вызывает обработку,
 * если данные корректны. Также обновляет качество соединения.
 */
void Connection_Net::ReadyRead()
{
    QByteArray data = socket->readAll();
    ///TODO check that buffer is clear
    QByteArray packageType= data.mid(3, 3);
    if ((packageType== "VTG" || packageType== "ZDA" || packageType== "GBS" ||
         packageType== "DTM" || packageType== "VLW" || packageType== "GNS" ||
         packageType== "GSV" || packageType== "GLL" || packageType== "GRS" ||
         packageType== "GSA" || packageType== "GST" || packageType== "VLW")) {
        qDebug() << packageType << " filtred" << data <<__FUNCTION__;
        data.clear();
        return; //ignoring packages if not GGA, RMC or HDT for not acepting them as bad packages
    }
    if (check_nmea_data(data)) { ///������������ �����������
        recieve_data(data);
        calcQuality(true);
        // qDebug() << "GOOD DATA "<< data;
    } else {
        calcQuality(false);
        // qDebug() << "BAD DATA "<< data;
    }
}

/**
 * @brief Слот, вызываемый при отключении соединения.
 *
 * Оповещает объект-владелец об отключении, выводит сообщение и отправляет сигнал о потере соединения.
 */
void Connection_Net::onDisconnected()
{
    qDebug() << "Disconnected from sender" <<IP;
    emit sendNewHasConnection(false);
}

/**
 * @brief Слот для обработки ошибок соединения.
 * @param socketError Код ошибки сокета.
 *
 * Выводит сообщение об ошибке и отправляет объекту-владельцу сигнал о потере соединения.
 */
void Connection_Net::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError;
    if (this->getTimer()->isActive()) {
        emit sendNewHasConnection(false);
    }
}
