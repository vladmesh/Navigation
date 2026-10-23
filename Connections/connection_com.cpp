#include "connection_com.h"

Connection_com::Connection_com()
{
}
/**
 * @brief Конструктор с параметрами для инициализации COM-порта и файла записи.
 * @param COM_port Имя COM-порта.
 * @param BaudRate Скорость передачи данных.
 * @param filename Имя файла для записи данных.
 *
 * Устанавливает параметры COM-порта, инициализирует файл записи,
 * если он указан и имеет расширение ".nmea".
 */
Connection_com::Connection_com(QString COM_port, int BaudRate, QString filename)
    : filename(filename), COM_port(COM_port),
    BaudRate(BaudRate)
{

    ui->setAttribute(Qt::WA_DeleteOnClose);

    if (!filename.isEmpty() && filename.endsWith(".nmea")) create_file_for_nmea(this->filename); //создание файла
    // Устанавливаем параметры порта
    SerialPort.setPortName(COM_port);
    SerialPort.setBaudRate(BaudRate);
    // SerialPort.setBaudRate(QSerialPort::Baud115200);

    SerialPort.setDataBits(QSerialPort::Data8);
    SerialPort.setParity(QSerialPort::NoParity);
    SerialPort.setStopBits(QSerialPort::OneStop);
    SerialPort.setFlowControl(QSerialPort::NoFlowControl);

    SerialPort.open(QIODevice::ReadOnly);
    connect(&SerialPort, &QSerialPort::readyRead, this,
            &Connection_com::ReadyRead);
    this->lastRecievedGGA = "empty com";

    ui->setWindowTitle("Port: " + COM_port);
}
/**
 * @brief Деструктор, закрывает COM-порт и файл записи.
 */
Connection_com::~Connection_com()
{
    SerialPort.close();
    file->close();
    // if (ui) delete ui;
}

QString Connection_com::getComPort()
{
    return COM_port;
}

int Connection_com::getBaudRate() const
{
    return BaudRate;
}
/**
 * @brief Переподключается к COM-порту, используя сохраненные параметры.
 *
 * Закрывает текущее подключение, сбрасывает настройки и повторно открывает порт.
 */
void Connection_com::reconnect()
{

    SerialPort.close();    // Устанавливаем параметры порта
    SerialPort.flush();
    SerialPort.setPortName(COM_port);
    SerialPort.setBaudRate(BaudRate);

    SerialPort.setDataBits(QSerialPort::Data8);
    SerialPort.setParity(QSerialPort::NoParity);
    SerialPort.setStopBits(QSerialPort::OneStop);
    SerialPort.setFlowControl(QSerialPort::NoFlowControl);

    SerialPort.open(QIODevice::ReadOnly);
    SerialPort.flush();
}


/**
 * @brief Обрабатывает данные, поступающие из COM-порта.
 *
 * Читает данные из порта, проверяет их корректность и, если данные корректны,
 * вызывает обработку, используя методы базового класса. При ошибках обновляет
 * качество соединения.
 */
void Connection_com::ReadyRead() {
    data.append(SerialPort.readAll());
    int index;
    if ((index = data.indexOf('\n')) == -1) {
        ;
    } else {
        QByteArray mainPart = data.left(index + 1);  // включая '\r\n'
        QByteArray removedPart = data.mid(index + 1); // после '\r\n'
        data = mainPart;
        QByteArray packageType= data.mid(3, 3);
        if ((packageType== "VTG" || packageType== "ZDA" || packageType== "GBS" ||
             packageType== "DTM" || packageType== "VLW" || packageType== "GNS" ||
             packageType== "GSV" || packageType== "GLL" || packageType== "GRS" ||
             packageType== "GSA" || packageType== "GST" || packageType== "VLW")) {
            qDebug() << packageType << " filtred" << data <<__FUNCTION__;
            data.clear();
            SerialPort.clear();
            return; //ignoring packages if not GGA, RMC or HDT for not acepting them as bad packages
        }
        if (check_nmea_data(data)) {
            recieve_data(data);
            calcQuality(true);
            qDebug() << "GOOD DATA "<< data;
        } else {
            calcQuality(false);
            qDebug() << "BAD DATA "<< data;
            SerialPort.clear();
        }
        data.clear();
        data = removedPart;
        // DataBuffer.clear();
    }
    // qDebug() <<"_____________";
}

