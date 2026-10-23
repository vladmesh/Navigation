#include "connection_creator.h"
#include "ui_connection_creator.h"


Connection_creator::Connection_creator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Connection_creator)
{
    ui->setupUi(this);

    updateCOMPortsList();

    ui->BRate_choose->setCurrentIndex(1);
    setWindowTitle("Create new connection");
    hideUDPPossibility();
}

Connection_creator::~Connection_creator()
{
    qDebug() << __FUNCTION__;

    for (int i = 0; i <  Connection_vector.size();i++) {
        QPointer<Connection> con = Connection_vector[i];
        delete con;
    }
    delete ui;

}

void Connection_creator::on_pushButton_clicked()
{


    Connection* newConnection = createConnection();
    if (newConnection) {
        Connection_vector.append(newConnection);
        newConnection->ui->show();

    }
    lastConnection = newConnection;
    accept();
    updateCOMPortsList();
    this->close();


}

Connection* Connection_creator::createConnection()
{
    QString fileName = this->dirPath.absolutePath()+"/"+lastCreatedFileName;
    if (!fileName.endsWith(".nmea", Qt::CaseInsensitive)) {
        fileName += ".nmea";
    }

    Connection* new_connection = nullptr;

    if (ui->connection_types->currentIndex() == 0) { // net connection
        QString IP = ui->lineEdit->text();
        uint16_t port = ui->lineEdit_2->text().toInt();
        if (ui->TCP_RB->isChecked()) {
            new_connection = new Connection_Net(IP, port, fileName);
        } else {
        }
    } else { // COM connection
        QString comport = ui->PortsAvailableComboBox->currentText();
        new_connection = new Connection_com(comport, ui->BRate_choose->currentText().toInt(), fileName);
    }

    lastConnection = new_connection;

    return new_connection;
}

Connection *Connection_creator::createConnection(QHostAddress IP, int port, QString fileName)
{
    auto new_connection = new Connection_Net(IP.toString(), port, fileName);
    lastConnection = new_connection;
    return new_connection;
}

Connection *Connection_creator::createConnection(QString COMport, int baudRate, QString fileName)
{
    auto new_connection = new Connection_com(COMport, baudRate, fileName);
    lastConnection = new_connection;
    return new_connection;
}

Connection *Connection_creator::getLastConnection()
{
    return lastConnection;
}

int Connection_creator::exec()
{
    updateCOMPortsList();
    return QDialog::exec();
}

void Connection_creator::on_connection_types_currentChanged(int index)
{
    if (index == 0) {
        ui->pushButton->setEnabled(true);
    } else {
        if (ui->PortsAvailableComboBox->count() == 0) {
            ui->pushButton->setEnabled(false);
        }
    }
    updateCOMPortsList();
}

void Connection_creator::hideUDPPossibility()
{
    ui->TCP_RB->hide();
    ui->UDP_RB->hide();
}

void Connection_creator::setLastCreatedFileName(const QString &newLastCreatedFileName)
{
    lastCreatedFileName = newLastCreatedFileName;
}

void Connection_creator::updateCOMPortsList()
{
    ui->PortsAvailableComboBox->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QVector<QString> busyPorts;
    for (auto i: Connection_vector) {
        if (i == nullptr) continue;
        if (QString(i->metaObject()->className()) == "Connection_com"){
            QPointer<Connection_com> conCom = qobject_cast<Connection_com*>(i.data());
            busyPorts.append(conCom->getComPort());
        }
    }

    for (const QSerialPortInfo &port : ports) {
        int flag = 0;
        for (auto i: busyPorts) {
            if (i == port.portName()) {
                flag = 1;
            }
        }
        if (flag) continue;
        ui->PortsAvailableComboBox->addItem(port.portName());
    }

}

void Connection_creator::setDirPath(const QDir &newDirPath)
{
    dirPath = newDirPath;
}


