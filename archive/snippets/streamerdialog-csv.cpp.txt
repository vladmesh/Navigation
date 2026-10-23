// Archived from Fabric/streamerdialog.cpp / streamerdialog.h. Commit: 4ac56dd (Initial import), identical in main aee8ae8.
// Feature: load/save streamer channel layout from/to CSV.
// Status: implemented, but buttons were never created (header said
// "не используется"). Known issues: saveToCSV writes hard-coded buoy depths,
// one line uses ',' instead of ';', readCSV does not read "Channel spacing"
// that saveToCSV writes.
//
// Declarations (class StreamerDialog):
// public:
//     void saveToCSV();
// private:
//     QDoubleSpinBox *elasticSectionLengthSpinBox = nullptr;
//     QPushButton* loadCSVButton = nullptr;
//     QPushButton* saveCSVButton = nullptr;
// private slots:
//     void loadCSVFile();
//     void readCSV(const QString& filePath);

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
