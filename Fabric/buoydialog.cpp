#include "buoydialog.h"

BuoyDialog::BuoyDialog(ItemsStorage* vault, bool needConnection)
    : MyVault(vault), needConnection(needConnection) {
    ///если без соединения, то к косе не привязан, т.е. он не концевой
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *spinBoxLayout = new QHBoxLayout();
    spinBoxLabel1 = new QLabel("Anthena Height:");
    spinBox1 = new QDoubleSpinBox();
    spinBox1->setRange(0.5, 2.5);
    spinBox1->setSingleStep(0.1);
    spinBox1->setValue(1.25);

    spinBoxLabel2 = new QLabel("Towing Depth:");
    spinBox2 = new QDoubleSpinBox();
    spinBox2->setRange(0.0, 5.0);
    spinBox2->setSingleStep(0.1);
    spinBox2->setValue(0.50);

    spinBoxLayout->addWidget(spinBoxLabel1);
    spinBoxLayout->addWidget(spinBox1);
    spinBoxLayout->addWidget(spinBoxLabel2);
    spinBoxLayout->addWidget(spinBox2);
    mainLayout->addLayout(spinBoxLayout);

    QPushButton *setButton = new QPushButton("Set");
    connect(setButton, &QPushButton::clicked, this, &BuoyDialog::onSetButtonClicked);
    mainLayout->addWidget(setButton);

    setLayout(mainLayout);
    setWindowTitle("Buoy parameters");
}


void BuoyDialog::onSetButtonClicked() {
    AnthenaHeight = spinBox1->value();
    TowingDepth = spinBox2->value();
    accept();
}
