#include "sendersettingsmenu.h"

SenderSettingsMenu::SenderSettingsMenu() {

    setWindowTitle("Sender Settings");
    setFixedSize(300, 150);

    // Поля ввода
    ipLineEdit = new QLineEdit("127.0.0.1", this);
    portSpinBox = new QSpinBox(this);
    portSpinBox->setMinimum(0);
    portSpinBox->setMaximum(20000);
    portSpinBox->setValue(1004);


    // Layout
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("IP Address:", ipLineEdit);
    formLayout->addRow("Port:", portSpinBox);

    // Кнопки
    QPushButton *okButton = new QPushButton("Start", this);
    QPushButton *cancelButton = new QPushButton("Stop", this);

    connect(okButton, &QPushButton::clicked, this, [this](){
        saveParams();
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, [this](){
        saveParams();
        reject();
    });

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SenderSettingsMenu::saveParams()
{
    // qDebug() << portSpinBox->value() << QHostAddress(ipLineEdit->text());
    portNumber = portSpinBox->value();
    IP = QHostAddress(ipLineEdit->text());
}


