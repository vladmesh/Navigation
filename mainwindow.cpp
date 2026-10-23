#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(this, &QMainWindow::destroyed, qApp, &QApplication::quit);

    ui->setupUi(this);
    hideDangerousButtons(); //прячем неугодное от взгяда солнцеликого клиента

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(894, 563);


    ///pointer to our list with item information
    tableWithItems = ui->ItemListTable;
    ///pointer to our list with item information

    ///Vault initialization
    Vault = new ItemsStorage();
    ///Vault initialization


    ///Fabric initialization
    MyFabric = new Fabric();
    MyFabric->setMyVault(Vault);
    ///Fabric initialization

    ///ObjectsListSetUp
    setUpObjectsTable();
    ///ObjectsListSetUp

    ///coordinator creation
    coordinator = new Coordinator();
    coordinator->Vault = this->Vault;
    ///coordinator creation

    ///p190Creator
    p190Creator = new P190_creator();
    p190Creator->setItemStoragePtr(Vault);

    DrawingAreaTopView = new MyGraphicView(this);
    ui->verticalLayout->addWidget(DrawingAreaTopView);
    DrawingAreaTopView->hide();
    DrawingAreaTopView->setVault(Vault);


    ///Painter
    paintingTimer = new QTimer(this);
    painter = new UberPainter(Vault);
    ui->verticalLayout->addWidget(painter);
    connect(paintingTimer, &QTimer::timeout, this, &MainWindow::runPainterRun);
    paintingTimer->start(1000);

    setMenuBar(menuBar());

    setWindowTitle("Seismic Navigation");


    ///one path to rule all the files
    ///and logger
    setPathForAllFiles();


    //наблюдатель за объектами
    objObserver = new ObjectObserver(Vault);
    objObserver->setTableWithItems(tableWithItems);


    P190HeaderAnalyzer.setVault(Vault);
    packageSender->setMyVault(Vault);
}

MainWindow::~MainWindow()
{
    qDebug() << __FUNCTION__;
    paintingTimer->stop();
    delete DrawingAreaTopView;
    if (Vault) delete Vault;
    delete MyFabric;
    delete ui;
    if (objObserver) delete objObserver;
}


void MainWindow::updateWiredWithComboBox()
{
    ui->ComboBoxWiredWith->clear();
    for (auto i: Vault->ItemsVault) {
        ui->ComboBoxWiredWith->addItem(i->name);
    }
}

void MainWindow::on_DeleteItemPushButton_clicked()
{
    paintingTimer->stop();
    int row = tableWithItems->currentRow();
    if (tableWithItems->rowCount() == 0) {
        QMessageBox msgBox;
        msgBox.warning(this, "Warning","No item to delete");
        return;
    }
    if (row == -1) row = tableWithItems->rowCount() - 1;
    QString deleteName = tableWithItems->item(row, 0)->text();
    recursiveItemDeleting(std::vector<QString>{deleteName});


    DrawingAreaTopView->update();
    paintingTimer->start();
    updateWiredWithComboBox();
}


void MainWindow::on_AddItemtPushButton_clicked()
{

    paintingTimer->stop(); //останавливаем таймер отрисовки пока ситуация не стабилизируется
    //ситуация с расчетом координат неопределенная, но вы держитесь там


    ///Creating New Obj
    QString oldname = ui->ItemNameLineEdit->text();
    QString name = GetNewDeviceName(oldname);
    ui->ItemNameLineEdit->setText(name);
    ///Creating New Obj

    FixedItem* newItem;

    if (ui->ItemTypeTabWidget->currentIndex() == 0) { ///getting params for fixed
        double x = ui->XCoorSpinBox->value();
        double y = ui->YCoorSpinBox->value();
        double z = ui->ZCoorSpinBox->value();
        FixedItemInfo FixedItemInfo(x,y,z,name);
        newItem = createFixedItem(FixedItemInfo);
    } else if (ui->ItemTypeTabWidget->currentIndex() == 1) {
        ///getting params for towed
        if(ui->ComboBoxWiredWith->count() == 0) {
            QMessageBox msgBox;
            msgBox.critical(this, "Error","No towing point");
            return;
        }

        FixedItem* twiw = Vault->getItem(
            ui->ComboBoxWiredWith->currentText());//toWhoIsWired
        float wireLength = ui->WireLengthSpinBox->value();
        float angleToWired = 270; //change further if needed
        QString itemType = ui->ComboBoxItemType->currentText();
        TowedItemInfo TowedItemInfo(twiw, wireLength, angleToWired, name);
        TowedItem* towedItem;

        if (itemType == "Streamer") {
            StreamerInfo StreamerItemInfo{TowedItemInfo, 0, {0}};
            towedItem = createStreamerItem(StreamerItemInfo);
        } else if (itemType == "Buoy") {
            BuoyInfo BuoyItemInfo{TowedItemInfo, -1,-1};
            towedItem = createBuoyItem(BuoyItemInfo);
        } else if (itemType == "Towed") {
            towedItem = createTowedItem(TowedItemInfo);
        } else if (itemType == "Source") {
            SourceInfo SourceItemInfo = {TowedItemInfo, -1, -1};
            towedItem = createSourceItem(SourceItemInfo);
        } else {
            fprintf(stderr, "no type for TowedItem");
            towedItem = nullptr;
        }
        newItem = towedItem;
    } else {
        return;
    }
    paintingTimer->start();//снова запускаем таймер отрисовки
    if (newItem == nullptr) return;

    //добавляем новый объект в список тех, за кем смотрит наблюдатель
    ui->ComboBoxWiredWith->addItem(newItem->name);
    objObserver->itemAdded(newItem);
    Vault->sortItemsByCalcPriority();

    DrawingAreaTopView->update();
    updateWiredWithComboBox();
    P190HeaderAnalyzer.analizeVault();
}


void MainWindow::setUpObjectsTable() {
    tableWithItems->setColumnCount(5);

    tableWithItems->setHorizontalHeaderLabels({"Name", "X", "Y", "Z", "State"});

    // Устанавливаем ширину столбцов 2 и 3 по умолчанию, равную 4 символам
    QFontMetrics fontMetrics(tableWithItems->font());
    int width = fontMetrics.horizontalAdvance('M') * 4;

    tableWithItems->setColumnWidth(1, width);  // 2-й столбец (индекс 1)
    tableWithItems->setColumnWidth(2, width);  // 3-й столбец (индекс 2)
    tableWithItems->setColumnWidth(3, width);

    // Устанавливаем режим "Interactive" для заголовков,
    // чтобы пользователь мог менять ширину столбцов
    tableWithItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // Оставшиеся столбцы можно растянуть на оставшееся место
    tableWithItems->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableWithItems->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    tableWithItems->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWithItems->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(tableWithItems, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);
}

void MainWindow::addItemToObjectsList(FixedItem* newItem) {

    QString name = newItem->name;
    float x = newItem->x;
    float y = newItem->y;
    float z = newItem->z;

    tableWithItems->insertRow(NumberOfRowsInTable);
    tableWithItems->setItem(NumberOfRowsInTable, 0, new QTableWidgetItem(name));
    tableWithItems->setItem(NumberOfRowsInTable, 1, new QTableWidgetItem(QString::number(x,'g',4)));
    tableWithItems->setItem(NumberOfRowsInTable, 2, new QTableWidgetItem(QString::number(y,'g',4)));
    tableWithItems->setItem(NumberOfRowsInTable, 3, new QTableWidgetItem(QString::number(z,'g',4)));
    tableWithItems->setItem(NumberOfRowsInTable, 4, new QTableWidgetItem(QString("")));


    for (int col = 0; col < 5; ++col) {
        QTableWidgetItem* item = tableWithItems->item(NumberOfRowsInTable, col);
        if (item) {
            if (col == 1 || col == 2 || col == 3 ) {
                // Разрешаем редактирование только для 2, 3 & 4 столбцовW
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            } else {
                // Отключаем редактирование для остальных столбцов
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    if (newItem->hasConnection == false) {
        tableWithItems->item(NumberOfRowsInTable, 4)->setText("None");
        tableWithItems->item(NumberOfRowsInTable, 4)->setBackground(QColor(Qt::lightGray));
    } else{

        tableWithItems->item(NumberOfRowsInTable, 4)->setBackground(QColor(Qt::yellow));
        tableWithItems->item(NumberOfRowsInTable, 4)->setText("Waiting");
    }

    NumberOfRowsInTable++;
}

void MainWindow::onItemChanged(QTableWidgetItem* item) {
    if (!tableWithItems->item(item->row(), 4)) return;
    if (tableWithItems->item(item->row(), 4)->text().isEmpty()) return;
    //защита от срабатывания при создании ячеек, т.к. текст в State появляется последним
    //костыль или решение, вот в чем вопрос

    int column = item->column();
    if (column == 1 || column == 2|| column == 3) { // Отслеживаем изменения во 2, 3 & 4 столбцах
        QString itemName = tableWithItems->item(item->row(), 0)->text();
        QPointer<FixedItem> editedItem = Vault->getItem(itemName);
        if (editedItem == nullptr) return;
        if (editedItem->itemType == "Fixed") {
            bool ok = true;
            tableWithItems->item(item->row(), column)->text().toDouble(&ok);
            if (!tableWithItems) return;
            if (ok) {
                editedItem->x = tableWithItems->item(item->row(), 1)->text().toDouble(&ok);
                editedItem->y = tableWithItems->item(item->row(), 2)->text().toDouble(&ok);
                editedItem->z = tableWithItems->item(item->row(), 3)->text().toDouble(&ok);
            }
        }
    }

    for (auto i: Vault->ItemsVault) {
        if (i->itemType == "Fixed") continue;
        QPointer<TowedItem> towed = QPointer(qobject_cast<TowedItem*>(i));
        if (towed->towingPoint == nullptr) {
            qWarning() << __FUNCTION__<<" no towing point";
            break;
        }
        towed->updateLayout();
        for (int i = 0; i < tableWithItems->rowCount(); i++) {
            if (tableWithItems->item(i, 0)->text() == towed->name) {
                tableWithItems->item(i, 1)->setText(QString::number(towed->x));
                tableWithItems->item(i, 2)->setText(QString::number(towed->y));
                tableWithItems->item(i, 3)->setText(QString::number(towed->z));
                break;
            }
        }
    }
    DrawingAreaTopView->update();
}

void MainWindow::saveLayoutToJson()
{
    paintingTimer->stop();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save configuration",
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Ship_logs/json", "Config Files (*.json);;All Files (*)");
    if (!fileName.isEmpty()) {
        QJsonObject params = ItemsLoader::instance().writeToJSON(this->Vault->ItemsVault);
        ItemsLoader::saveJsonObjectToFile(params, fileName);
    }
    paintingTimer->start();
}

void MainWindow::destroyAllTheObjects()
{
    while (tableWithItems->rowCount() != 0) {
        emit on_DeleteItemPushButton_clicked(); //оно так лучше работает
        //вся логика уже прописана
    }
}


void MainWindow::closeEvent(QCloseEvent *event) {
    // Создаём QMessageBox для подтверждения закрытия
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Closing app");
    msgBox.setText("Are you sure?");
    msgBox.setIcon(QMessageBox::Question);

    // Добавляем стандартные кнопки Yes и No
    QPushButton *yesButton = msgBox.addButton(QMessageBox::Yes);
    QPushButton *noButton = msgBox.addButton(QMessageBox::No);

    // Добавляем пользовательскую кнопку Save config
    QPushButton *saveButton = msgBox.addButton("Save config", QMessageBox::ActionRole);

    // Показываем сообщение и ждём пользовательского выбора
    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        qDebug() << "Closing...";
        event->accept();
        destroyAllTheObjects();
    } else if (msgBox.clickedButton() == saveButton) {
        qDebug() << "Saving config...";
        saveLayoutToJson();
        event->accept();
        destroyAllTheObjects();
    } else if (msgBox.clickedButton() == noButton) {
        qDebug() << "Cancel closing.";
        event->ignore();
    }
}


void MainWindow::setMenuBar(QMenuBar *menuBar) //здесь мы создаем окна менюшки
{
    // Проверяем и создаем меню "File", если его еще нет
    QMenu *fileMenu = menuBar->findChild<QMenu*>("fileMenu");
    if (!fileMenu) {
        fileMenu = new QMenu("File", menuBar);
        fileMenu->setObjectName("fileMenu");
        menuBar->addMenu(fileMenu);
    } else {
        fileMenu->clear();
    }

    // Создание действия "Сохранить параметры"
    QAction *saveAction = new QAction("Save configuration", fileMenu);
    QObject::connect(saveAction, &QAction::triggered, menuBar->parent(), [this]() {
        saveLayoutToJson();
    });
    fileMenu->addAction(saveAction);


    QAction *loadAction = new QAction("Load configuration", fileMenu);
    QObject::connect(loadAction, &QAction::triggered, menuBar->parent(), [this]() {
        paintingTimer->stop();
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load configuration",
             QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Ship_logs/json", "Config Files (*.json);;All Files (*)");

        if (!fileName.isEmpty()) {
            QFile* file = new QFile(fileName);
            loadLayout(file);
        }
        paintingTimer->start();
    });
    fileMenu->addAction(loadAction);

    QAction *postProcessingAction = new QAction("Start postprocessing", fileMenu);
    QObject::connect(postProcessingAction, &QAction::triggered, menuBar->parent(), []() {
        PostProcessorView* postProcView = PostProcessorView::getInstance();
        postProcView->show();
    });
    fileMenu->addAction(postProcessingAction);


    QAction *startSendingCoordinatesAction = new QAction("Send coordinates", fileMenu);
    QObject::connect(startSendingCoordinatesAction, &QAction::triggered, menuBar->parent(), [this]() {
        SenderSettingsMenu dialog;
        if (dialog.exec() == QDialog::Accepted) {
            if (dialog.IP != packageSender->getIP() || dialog.portNumber != packageSender->getPort()) {
                qDebug() << dialog.IP << dialog.portNumber;
                packageSender->setIP(dialog.IP);
                packageSender->setPort(dialog.portNumber);
                packageSender->connectToHost();
            }
            packageSender->setNeedSendPackage(true);
        } else {
            packageSender->setNeedSendPackage(false);
        }
    });
    fileMenu->addAction(startSendingCoordinatesAction);

    // Создаем меню "About" и добавляем в него пункт "Help"
    QMenu* aboutMenu = new QMenu("About", menuBar);
    aboutMenu->setObjectName("aboutMenu");

    QAction *helpAction = new QAction("Help", aboutMenu);
    QObject::connect(helpAction, &QAction::triggered, menuBar->parent(), []() {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Info");
        msgBox.setText(QString("Version 1.0\nReleased at %1\nValid till 31.12.2025")
                           .arg(__DATE__ ));
        QPixmap logoPixmap(":/Data/logoGeodevice.svg");
        QPixmap scaledLogo = logoPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        msgBox.setIconPixmap(scaledLogo);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    });
    aboutMenu->addAction(helpAction);
    menuBar->addMenu(aboutMenu);


    // Создаем меню "Settings"
    QMenu *settingsMenu = menuBar->findChild<QMenu*>("settingsMenu");
    if (!settingsMenu) {
        settingsMenu = new QMenu("Settings", menuBar);
        settingsMenu->setObjectName("settingsMenu");
        menuBar->addMenu(settingsMenu);
    } else {
        settingsMenu->clear();
    }

    // Добавляем пункт "P1/90 Header" в меню "Settings"
    QAction *p190HeaderAction = new QAction("P1/90 Header", settingsMenu);
    QObject::connect(p190HeaderAction, &QAction::triggered, menuBar->parent(), [this]() {
        P190HeaderChanger *headerChanger = new P190HeaderChanger();
        headerChanger->setVault(Vault);
        headerChanger->show();
    });
    QAction* shipParamsChangerAction = new QAction("Ship Settings", settingsMenu);
    QObject::connect(shipParamsChangerAction, &QAction::triggered, menuBar->parent(), [this]() {
        ShipParamsChanger* shipParamChanger = new ShipParamsChanger(nullptr, DrawingAreaTopView->MyShip);
        if (shipParamChanger->exec() == QDialog::Accepted) {
            QVector<QPointF> tmp = shipParamChanger->getFigureCoordinates();
            for (auto i: shipParamChanger->getFigureCoordinates()) {
                qDebug() << i;
            }

            DrawingAreaTopView->setShipFig(tmp);
            DrawingAreaTopView->updateShip();
            DrawingAreaTopView->update();
            coordinator->boardDepthChanged(shipParamChanger->getMainDeckHeight());
        }
        delete shipParamChanger;
    });

    settingsMenu->addAction(p190HeaderAction);
    settingsMenu->addAction(shipParamsChangerAction);
}


void MainWindow::loadLayout(QFile* jsonFile)
{
    destroyAllTheObjects();

    ItemsLoader::dirpath = pathForAllFiles;
    QStringList loadErrors;
    QVector<QPointer<FixedItem>> items = ItemsLoader::readFromJSON(jsonFile, &loadErrors);
    if (!loadErrors.isEmpty()) {
        QMessageBox::warning(this, "Broken layout",
                             "Some objects of the layout were not loaded:\n" + loadErrors.join("\n"));
    }
    Vault->deleteAllItems();
    for (QPointer<FixedItem> p:items) {
        addItemToObjectsList(p); //в таблицу
        Vault->ItemsVault.push_back(p); //в хранилище
        ui->ComboBoxWiredWith->addItem(p->name); //список тех, с кем можно связаться
        objObserver->itemAdded(p); // отслеживание состояний
    }
    Vault->sortItemsByCalcPriority();
    DrawingAreaTopView->update();
}

void MainWindow::setPathForAllFiles()
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString folderName = QString("%1_%2__%3_%4")
                             .arg(currentDateTime.date().day(),2, 10, QChar('0'))
                             .arg(currentDateTime.date().month(),2, 10, QChar('0'))
                             .arg(currentDateTime.time().hour())
                             .arg(currentDateTime.time().minute(), 2, 10, QChar('0'));


    QDir dir(dirPath + "/Ship_logs/" + folderName);
    dir.mkpath(dirPath + "/Ship_logs/" + folderName);
    this->pathForAllFiles = dir;

    if (MyFabric) MyFabric->connectionCreator->setDirPath(dir);
    if (p190Creator) p190Creator->setPath(dir);

}


void MainWindow::hideDangerousButtons()
{
    // ui->SideViewRB->hide();
    // ui->TopViewRB->hide();
    // ui->StartP190CreationOnlinePB->hide();
    // ui->DeleteItemPushButton->hide();
    // ui->ReconnectPB->hide();
    // ui->DeleteItemPushButton->hide();
}


FixedItem* MainWindow::createFixedItem(FixedItemInfo NewItemInfo) {
    bool needConnection = false;

    if (ui->NeedConnectionCB->isChecked()) {
        needConnection = true;
    }


    FixedItem* NewItem = MyFabric->CreateItem(NewItemInfo, needConnection);
    if (NewItem == nullptr) return nullptr;

    ///Saving New Item
    Vault->SaveItem(NewItem);
    ///Saving New Item

    ///adding obj to table
    addItemToObjectsList(NewItem);
    ///adding obj to table


    ///adding new object to our Drawing area
    ///adding new object to our Drawing area

    return NewItem;
}

TowedItem* MainWindow::createTowedItem(TowedItemInfo NewItemInfo) {
    bool needConnection = false;

    if (ui->NeedConnectionCB->isChecked()) {
        needConnection = true;
    }
    TowedItem* NewItem = MyFabric->CreateItem(NewItemInfo, needConnection);
    if (NewItem == nullptr) return nullptr;
    ///Saving New Item
    Vault->SaveItem(NewItem);
    ///Saving New Item


    ///adding obj to table
    addItemToObjectsList(NewItem);
    ///adding obj to table


    ///adding new object to our Drawing area
    ///adding new object to our Drawing area


    return NewItem;
}

Streamer* MainWindow::createStreamerItem(StreamerInfo info)
{

    Streamer* NewItem = MyFabric->CreateItem(info);
    if (NewItem == nullptr) return nullptr;

    ///Saving New Item
    Vault->SaveItem(NewItem);
    ///Saving New Item

    ///adding obj to table
    addItemToObjectsList(NewItem);
    ///adding obj to table

    ///Drawing
    drawLineToTowed(NewItem);


    return NewItem;


}

Buoy* MainWindow::createBuoyItem(BuoyInfo BuoyItemInfo)
{
    bool needConnection = false;
    if (ui->NeedConnectionCB->isChecked()) {
        needConnection = true;
    }

    Buoy* NewItem = MyFabric->CreateItem(BuoyItemInfo, needConnection);
    if (NewItem == nullptr) return nullptr;
    if (QString(BuoyItemInfo.towedInfo.toWhoIsWired->metaObject()->className()) == "Streamer") {
        dynamic_cast<Streamer*>(Vault->getItem(BuoyItemInfo.towedInfo.toWhoIsWired->name))->setEndBuoy(NewItem);
    }
    //тут мы проверяем, что если буй привязан к косе, то коса->концевой буй* = новый созданный объект
    //а потом увеличиваем дистанцию буксировки на длину косы


    ///Saving New Item
    Vault->SaveItem(NewItem);
    ///Saving New Item

    ///adding obj to table
    addItemToObjectsList(NewItem);
    ///adding obj to table

    ///adding ability to wire any towed item with it
    ui->ComboBoxWiredWith->addItem(NewItem->name);

    return NewItem;
}

Source* MainWindow::createSourceItem(SourceInfo SourceItemInfo) //метод для создания объекта под источник
{
    bool needConnection = false;
    if (ui->NeedConnectionCB->isChecked()) {
        needConnection = true;
    }


    Source* NewItem = MyFabric->CreateItem(SourceItemInfo, needConnection);
    if (NewItem == nullptr) return nullptr;
    ///Saving New Item
    Vault->SaveItem(NewItem);
    ///Saving New Item

    ///adding obj to table
    addItemToObjectsList(NewItem);
    ///adding obj to table

    ///adding ability to wire any towed item with it
    ui->ComboBoxWiredWith->addItem(NewItem->name);

    return NewItem;
}


QString MainWindow::GetNewDeviceName(QString name) {
    //делает невозможным повторение имен
    //если имена повторяются, добавляет постфикс _1 _2 _3 и тд
    for (FixedItem* item: Vault->ItemsVault) {
        if (name == item->name) {
            if (name.at(name.length()-1).isDigit()){
                int curNumber = name.mid(1+name.lastIndexOf("_")).toInt();
                return name.left(name.lastIndexOf("_")) + "_" + QString::number(curNumber+1);
            } else {
                return name + "_1";
            }
        }
    }
    return name;
}

void MainWindow::recursiveItemDeleting(std::vector<QString> names) {
    if (names.size() == 0) return;
    std::vector<QString> res;

    for (auto name: names) {
        for (auto item:Vault->getItem(name)->vectorOfConnected) {
            if (item == nullptr) continue;
            res.push_back(item->name);
        }
    }
    recursiveItemDeleting(res);
    for (auto name:names) {
        deleteItemByNameFromUI(name);
    }
}

void MainWindow::deleteItemByNameFromUI(QString name) {
    if (paintingTimer->isActive()) paintingTimer->stop();
    int row = -1;
    for (int i = 0; i < tableWithItems->rowCount(); i++) {
        if (tableWithItems->item(i, 0)->text() == name) {
            row = i;
            break;
        }
    }
    if (row == -1) return;
    painter->eraseFromPainting(Vault->getItem(name));
    tableWithItems->removeRow(row);
    Vault->DeleteItem(name);
    ui->ComboBoxWiredWith->removeItem(ui->ComboBoxWiredWith->findText(name));
    NumberOfRowsInTable--;

    if (!paintingTimer->isActive()) paintingTimer->start();
}


void MainWindow::drawLineToTowed(TowedItem* item) {
    //рисует линию от буксируемого устройства к точке крепления
    if (QString(item->towingPoint->metaObject()->className()) == "Streamer") {
        float streamerLength = dynamic_cast<Streamer*>(item->towingPoint)->totalLength;
        DrawingAreaTopView->drawLineToTowed(item->x, item->y,
                                            item->towingPoint->x-streamerLength, item->towingPoint->y);
    } else {
        DrawingAreaTopView->drawLineToTowed(item->x, item->y,
            item->towingPoint->x, item->towingPoint->y);
    }
}

void MainWindow::on_TopViewRB_clicked()
{
    DrawingAreaTopView->show();
    painter->hide();
}


void MainWindow::on_SideViewRB_clicked()
{
    DrawingAreaTopView->hide();
    painter->show();
}


void MainWindow::on_ComboBoxItemType_activated(int index)
{
    switch(index) {
    case 0:
        ui->ItemNameLineEdit->setText("Towed");
        ui->NeedConnectionCB->setChecked(false);
        ui->NeedConnectionCB->setEnabled(true);
        break;
    case 1:
        ui->ItemNameLineEdit->setText("Source");
        ui->NeedConnectionCB->setEnabled(true);
        break;
    case 2:
        ui->ItemNameLineEdit->setText("Buoy");
        ui->NeedConnectionCB->setChecked(false);
        ui->NeedConnectionCB->setEnabled(true);
        break;
    case 3:
        ui->ItemNameLineEdit->setText("Streamer");
        ui->NeedConnectionCB->setChecked(false);
        ui->NeedConnectionCB->setEnabled(false);
        break;
    default:
        ui->ItemNameLineEdit->setText("");
        ui->NeedConnectionCB->setEnabled(true);
        break;
    }
}


void MainWindow::runPainterRun()
{
    coordinator->calcCoors();
    painter->run();

}


//для того, чтобы абсолютный лэйбек не был больше абс.лэйбека у того объекта,
//за котоорым мы буксируемся. особая логика для кос по понятным причинам
void MainWindow::on_ComboBoxWiredWith_currentTextChanged(const QString &arg1)
{
    FixedItem* ourItem(Vault->getItem(arg1));
    if (ourItem == nullptr) return;
    if (ourItem->itemType == "Fixed") {
        ui->WireLengthSpinBox->setMinimum(0);
        ui->WireLengthSpinBox->setValue(0);
    } else if (ourItem->itemType == "Streamer") {
        float minLen = qobject_cast<Streamer*>(ourItem)->absoluteLayback;
        float streamerLen = qobject_cast<Streamer*>(ourItem)->totalLength;
        ui->WireLengthSpinBox->setMinimum(minLen+streamerLen);
        ui->WireLengthSpinBox->setValue(minLen+streamerLen);
    } else {
        float minLen = qobject_cast<TowedItem*>(ourItem)->absoluteLayback;
        ui->WireLengthSpinBox->setMinimum(minLen);
        ui->WireLengthSpinBox->setValue(minLen);
    }
}

