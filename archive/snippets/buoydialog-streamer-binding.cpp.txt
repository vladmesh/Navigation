// Archived from Fabric/buoydialog.cpp / buoydialog.h. Commit: 4ac56dd (Initial import), identical in main aee8ae8.
// Feature: choose the streamer for an end buoy in BuoyDialog via a combo box.
// Status: commented out; the binding is now done through Streamer::setEndBuoy
// (MainWindow::createBuoyItem, ItemsLoader). Looks like an abandoned approach.
//
// Fields that were in class BuoyDialog:
//     QComboBox *comboBox;
//     QString selectedStreamerName = "";
//     Streamer* selectedStreamer = nullptr;
//
// From BuoyDialog::BuoyDialog:

    // QHBoxLayout *comboBoxLayout = new QHBoxLayout();

    // if (needConnection){
    //     QLabel *comboBoxLabel = new QLabel("Streamer Items:");
    //     comboBox = new QComboBox();
    //     comboBoxLayout->addWidget(comboBoxLabel);
    //     comboBoxLayout->addWidget(comboBox);
    //     mainLayout->addLayout(comboBoxLayout);
    //     for (const auto &item : vault->ItemsVault) {
    //         if (QString(item->metaObject()->className()) ==  "Streamer") {
    //             comboBox->addItem(item->name);
    //         }
    //     }
    // }

// From BuoyDialog::onSetButtonClicked:
    // if (needConnection) {
    //     if (comboBox->count() == 0) {
    //         reject();
    //     } else {
    //         selectedStreamerName = comboBox->currentText();
    //         selectedStreamer = qobject_cast<Streamer*>(MyVault->getItem(selectedStreamerName));
    //     }
    //     if (selectedStreamerName.isEmpty()) {
    //         //просто буй, возможно leadBuoy
    //     } else {
    //         qDebug() << MyVault->getItem(selectedStreamerName)->name << "buoyDialog";
    //     }
    // }
