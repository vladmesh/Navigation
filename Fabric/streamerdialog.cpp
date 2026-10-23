#include "streamerdialog.h"

StreamerDialog::StreamerDialog(QWidget* parent)
    : QDialog(parent), elasticSectionLength(12.5) {
    setFixedSize(400, 250);
    tabWidget = new QTabWidget(this);

    // Создаём окно состояния
    statusLabel = new QLabel("Status: Ready", this);

    // Single Channel Tab
    singleChannelTab = new QWidget(this);
    singleChannelComboBox = new QComboBox(singleChannelTab);
    singleChannelComboBox->addItems({
        "HRStreamer-1×8",
        "HRStreamer-1×16",
        "HRStreamer-1×24",
        "HRStreamer-1×48",
        "HRStreamer-1×96"
    });

    QVBoxLayout *singleChannelLayout = new QVBoxLayout(singleChannelTab);
    singleChannelLayout->addWidget(singleChannelComboBox);
    singleChannelLayout->addStretch(1);  // Bottom spacing
    singleChannelTab->setLayout(singleChannelLayout);

    // Multi Channel Tab
    multiChannelTab = new QWidget(this);
    QLabel *channelsLabel = new QLabel("Number of channels", multiChannelTab);
    multiChannelComboBoxChannels = new QComboBox(multiChannelTab);
    multiChannelComboBoxChannels->addItems({"24", "48"});

    QLabel *distanceLabel = new QLabel("Step between channels", multiChannelTab);
    multiChannelComboBoxDistance = new QComboBox(multiChannelTab);
    multiChannelComboBoxDistance->addItems({"1", "2", "3.125"});

    QVBoxLayout *multiChannelLayout = new QVBoxLayout(multiChannelTab);
    multiChannelLayout->addWidget(channelsLabel);
    multiChannelLayout->addWidget(multiChannelComboBoxChannels);
    multiChannelLayout->addWidget(distanceLabel);
    multiChannelLayout->addWidget(multiChannelComboBoxDistance);
    multiChannelLayout->addStretch(1);  // Bottom spacing
    multiChannelTab->setLayout(multiChannelLayout);

    // Additional option for multi-channel tab
    connect(multiChannelComboBoxChannels, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        multiChannelComboBoxDistance->clear();
        if (index == 1) { // 48 channels selected
            multiChannelComboBoxDistance->addItems({"1&2", "1", "2", "3.125", "6.25"});
        } else { // 24 channels selected
            multiChannelComboBoxDistance->addItems({"1", "2", "3.125", "6.25"});
        }
        updateStatusLabel();
    });

    connect(multiChannelComboBoxDistance, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StreamerDialog::updateStatusLabel);

    tabWidget->addTab(multiChannelTab, "Multi channel");
    tabWidget->addTab(singleChannelTab, "Single channel");

    connect(singleChannelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StreamerDialog::updateStatusLabel);


    connect(tabWidget, &QTabWidget::currentChanged, this, &StreamerDialog::updateStatusLabel);



    multiChannelComboBoxDistance->setCurrentIndex(2);
    multiChannelComboBoxChannels->setCurrentIndex(1);

    multiChannelTab->setFocus();

    finishButton = new QPushButton("Finish", this);
    connect(finishButton, &QPushButton::clicked, this, &StreamerDialog::onFinishButtonClicked);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(statusLabel);  // Добавляем окно состояния в layout
    mainLayout->addWidget(finishButton);
    mainLayout->setAlignment(finishButton, Qt::AlignRight); // Align finish button to the right
    setLayout(mainLayout);
    setWindowTitle("Streamer parameters");


    connect(multiChannelComboBoxDistance, &QComboBox::currentIndexChanged, this, &StreamerDialog::updateStatusLabel);
    connect(multiChannelComboBoxChannels, &QComboBox::currentIndexChanged, this, &StreamerDialog::updateStatusLabel);
    connect(singleChannelComboBox, &QComboBox::currentIndexChanged, this, &StreamerDialog::updateStatusLabel);

}

void StreamerDialog::onFinishButtonClicked()
{
    // qDebug() << "StreamerDialog::onFinishButtonClicked";
    createChansValues();
    elasticSectionLength = 0;
    leadingDamperLength = elasticSectionLength;
    endDamperLength = 0;
    totalLength = leadingDamperLength + endDamperLength + chans.last() - chans.first();
    accept();
}

void StreamerDialog::updateStatusLabel()
{
    QString status;
    createChansValues();
    float totalLength = chans.at(chans.size()-1);
    statusLabel->setText("Total Length: " + QString::number(totalLength) + " m.");
}

void StreamerDialog::loadCSVFile() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv)"));
    if (!filePath.isEmpty()) {
        readCSV(filePath);
    } else {
        QMessageBox::warning(this, tr("Error"), tr("File Is Empty"));
        return;
    }
}

void StreamerDialog::readCSV(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file"));
        return;
    }

    QTextStream in(&file);
    chans.clear();
    numOfChannels = 0;
    float leadingDamperLength = 0.0;
    float endDamperLength = 0.0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(';');
        bool ok;
        if (fields.size() != 2) continue; // Skip lines with unexpected number of fields

        if (fields[0] == "leading damper section length (m)") {
            leadingDamperLength = fields[1].toFloat();
        } else if (fields[0] == "end damper section length (m)") {
            endDamperLength = fields[1].toFloat();
        } else if (fields[0] == "Channels count") {
            numOfChannels = fields[1].toInt();
        } else if (fields[0].toInt(&ok) > 0 && ok) { // Assume first field is channel number
            chans.append(fields[1].toFloat());
        }
    }

    if (!chans.isEmpty()) {
        totalLength = leadingDamperLength + endDamperLength + chans.last() - chans.first();
        QMessageBox::information(this, tr("Success"), tr("CSV file loaded successfully."));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("CSV file is empty or invalid."));
    }
    accept();
}

void StreamerDialog::saveToCSV() {
    // Generate file name
    QString baseFileName;
    if (tabWidget->currentIndex() == 0) {
        baseFileName = singleChannelComboBox->currentText() + ".csv";
    } else {
        baseFileName = QString("HRStreamer_%1_%2.csv").arg(numOfChannels).arg(chans.isEmpty() ? 0 : chans[0]);
    }
    QFile file(baseFileName);
    int suffix = 1;
    createChansValues();
    // Check if file already exists and add suffix if necessary
    while (file.exists()) {
        baseFileName = QString("%1_new%2.csv").arg(baseFileName.left(baseFileName.lastIndexOf('.'))).arg(suffix);
        file.setFileName(baseFileName);
        ++suffix;
    }
    // Open file for writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create file"));
        return;
    }

    QTextStream out(&file);

    // Write headers and values to the file
    out << "leading damper section length (m);" << leadingDamperLength << "\n";
    out << "end damper section length (m);" << endDamperLength << "\n";
    out << "Channel spacing (m);" << (chans.size() > 1 ? chans[1] - chans[0] : 0) << "\n";
    out << "Channels count;" << numOfChannels << "\n";
    out << "Leading buoy towing depth (m),0.5\n";
    out << "End buoy towing depth (m);1.5\n";
    out << "Distance to end buoy (m);3\n\n";
    out << "Channel number;offset from leading buoy\n";

    for (int i = 0; i < chans.size(); ++i) {
        out << i + 1 << ";" << chans[i] << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Success"), tr("CSV file saved successfully."));
}

void StreamerDialog::createChansValues()
{
    chans.clear();//на случай если несколько раз окно вызываем
    if (tabWidget->currentIndex() == 0) { //multichannel streamer
        bool ok;
        numOfChannels = multiChannelComboBoxChannels->currentText().toUInt();
        float dCh = multiChannelComboBoxDistance->currentText().toFloat(&ok);
        if (ok) {
            for (uint i = 0; i < numOfChannels; i++) {
                chans.append(i*dCh);
            }
            dChString = QString::number(dCh);
        } else { //1&2
            dCh = 1;
            for (uint i = 0; i < 24; i++) {
                chans.append(i*dCh);
            }
            dCh = 2;
            for (uint i = 0; i < 24; i++) {
                chans.append(chans[23]+i*dCh + 2);
            }
            dChString = "1&2";
        }
    } else{ //HRStreamer-1
        // float dCh = 0.2;
        dChString = "";
        switch(singleChannelComboBox->currentIndex()){
        case 0:
            numOfChannels = 2;
            chans.append(0);
            chans.append(chans[0] + 0.8);
            // qDebug() << "1x8";
            singleChannelType = "1x8";
            break;
        case 1:
            numOfChannels = 3;
            chans.append(0);
            chans.append(chans[0] + 0.8);
            chans.append(chans[1] + 1.6);
            // qDebug() << "1x16";
            singleChannelType = "1x16";
            break;
        case 2:
            numOfChannels = 3;
            chans.append(0);
            chans.append(chans[0] + 0.8);
            chans.append(chans[1] + 2.4);
            // qDebug() << "1x24";
            singleChannelType = "1x24";
            break;
        case 3:
            numOfChannels = 4;
            chans.append(0);
            chans.append(chans[0] + 0.8);
            chans.append(chans[1] + 2.4);
            chans.append(chans[2] + 4);
            // qDebug() << "1x48";
            singleChannelType = "1x48";
            break;
        case 4:
            numOfChannels = 5;
            chans.append(0);
            chans.append(chans[0] + 0.8);
            chans.append(chans[1] + 2.4);
            chans.append(chans[2] + 4);
            chans.append(chans[3] + 7.2);
            // qDebug() << "1x96";
            singleChannelType = "1x96";
            break;
        default:
            break;
        }
    }
}


