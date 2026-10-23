#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicview.h"
#include "fabric.h"
#include "itemsstorage.h"
#include "itemsloader.h"
#include "coordinator.h"
#include "p190_creator.h"
// #include "logger.h"
#include "postprocessor.h"
#include "segyreader.h"
#include "postprocessorview.h"
#include "uberpainter.h"
#include "objectobserver.h"
#include "p190headerchanger.h"
#include "shipparamschanger.h"
#include "sendersettingsmenu.h"
#include "sender.h"


#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QTableWidget>
#include <QRegularExpression>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


/**
 * @class MainWindow
 * @brief Главное окно приложения, управляющее созданием, хранением и взаимодействием объектов.
 *
 * Этот класс отвечает за весь цикл управления данными приложения:
 * - Создание и настройка объектов (фиксированных и буксируемых устройств).
 * - Организация хранения данных объектов через ItemsStorage.
 * - Интеграция с визуализацией объектов через графический интерфейс.
 * - Реализация логики взаимодействия между объектами и их визуальными представлениями.
 * - Управление обработкой данных навигации и результатов сейсмической съемки.
 *
 * Главные функции класса:
 * - Управление основными компонентами, включая фабрику объектов, хранилище, наблюдателей и логгер.
 * - Настройка пользовательского интерфейса, включая таблицу объектов и графическое отображение.
 * - Обеспечение корректного взаимодействия объектов, включая расчет координат и отрисовку.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор главного окна.
     * здесь создаются экземпляры используемых классов
     * запускается таймер отрисовки объектов
     * инициализируется хранилище объектов и
     * @param parent Родительский виджет.
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    /**
     * @brief Создает фиксированный объект.
     * @param FixedItemInfo Параметры для создания фиксированного объекта.
     * @return Указатель на созданный объект.
     */
    FixedItem* createFixedItem(FixedItemInfo FixedItemInfo);
    /**
     * @brief Создает буксируемый объект.
     * @param TowedItemInfo Параметры для создания буксируемого объекта.
     * @return Указатель на созданный объект.
     */
    TowedItem* createTowedItem(TowedItemInfo TowedItemInfo);
    /**
     * @brief Создает объект сейсмической косы (стримера).
     * @param StreamerItemInfo Параметры для создания стримера.
     * @return Указатель на созданный объект.
     */
    Streamer* createStreamerItem(StreamerInfo StreamerItemInfo);
    /**
     * @brief Создает объект типа буй.
     * @param BuoyItemInfo Параметры для создания буя.
     * @return Указатель на созданный объект.
     */
    Buoy* createBuoyItem(BuoyInfo BuoyItemInfo);
    /**
     * @brief Создает объект типа источник.
     * @param SourceItemInfo Параметры для создания источника.
     * @return Указатель на созданный объект.
     */
    Source* createSourceItem(SourceInfo SourceItemInfo);
    /**
     * @brief Генерирует уникальное имя устройства, если предложенное имя уже существует.
     * @param name Предложенное имя.
     * @return Уникальное имя.
     */
    QString GetNewDeviceName(QString name);

    /**
     * @brief рекурсивно удаляет объект и связанные с ним объекты
     * @param names имена объектов что надо кильнуть
     */
    void recursiveItemDeleting(std::vector<QString> names);
private:

    QPointer<Sender> packageSender = new Sender();

    /**
     * @brief Рисует линию от буксируемого объекта до точки крепления.
     * @param item Указатель на буксируемый объект.
     */
    void drawLineToTowed(TowedItem* item);

    /**
     * @brief Рисует графическое представление стримера.
     * @param item Указатель на объект стримера.
     */
    void drawStreamer(Streamer* item);

    /**
     * @brief Добавляет объект в список объектов в графическом интерфейсе.
     * @param newItem Указатель на добавляемый объект.
     */
    void addItemToObjectsList(FixedItem* newItem);

    /**
     * @brief Настраивает таблицу объектов в пользовательском интерфейсе.
     */
    void setUpObjectsTable();

    /**
     * @brief Устанавливает меню главного окна.
     * @param menuBar Указатель на строку меню.
     */
    void setMenuBar(QMenuBar *menuBar);

    /**
     * @brief Загружает конфигурацию из JSON файла.
     * @param jsonFile Указатель на JSON файл.
     */
    void loadLayout(QFile* jsonFile);

    ///Path for all generated files
    QDir pathForAllFiles = QDir(); ///<Единый путь для всех объектов создающих данные
    /**
     * @brief Устанавливает путь для всех генерируемых файлов.
     */
    void setPathForAllFiles();

    ///Path for all generated files

    ///TODO
    void CreateShipItem();
    ///TODO

    Ui::MainWindow *ui;

    P190_creator* p190Creator = nullptr;
    MyGraphicView *DrawingAreaTopView = nullptr;

    QTimer* paintingTimer = nullptr;
    UberPainter* painter = nullptr;
    //кароч, делал класс рисовальщика положений наших объектов
    //оказалось, нельзя UI в отельный поток засовывать
    //придется делать таймер, по которому будет рисование

    Fabric* MyFabric = nullptr;
    ItemsStorage* Vault = nullptr;
    QTableWidget* tableWithItems = nullptr;
    Coordinator* coordinator = nullptr;
    QTimer* MyTimer = nullptr;
    // Logger* logger = nullptr;

    int NumberOfRowsInTable = 0;

    // SegYReader* sr = nullptr;
    // void setSegyReader();
    // QVector <SegYReader::SegyData> pairs;
    // int curSegyFile = 0;

    ///<наблюдатель за объектами и соединениями
    ///<когда соединение ломается/чинится, через систему
    ///<сигналов слотов идет информация о том, в каком объекте
    ///<заодно и меняется hasConnection, а от этого - логика рассчета координат
    ObjectObserver* objObserver = nullptr;

    p190HeaderAnalyzer P190HeaderAnalyzer;

    /**
     * @brief Скрывает неиспользуемые или недоработанные кнопки интерфейса.
     */
    void hideDangerousButtons();
    void deleteItemByNameFromUI(QString name);
private slots:
    void on_DeleteItemPushButton_clicked();
    void on_AddItemtPushButton_clicked();

    void timeToCollectData(); ///< Обновляет данные объектов по таймеру.

    void on_TopViewRB_clicked();
    void on_SideViewRB_clicked();
    void on_ComboBoxItemType_activated(int index);
    // void on_doubleSpinBox_valueChanged(double arg1);
    // void on_pushButton_clicked();
    // void on_pushButton_2_clicked();



    void runPainterRun(); ///< Запускает обновление отрисовки объектов классом UberPainter

    void onItemChanged(QTableWidgetItem* item);

    void saveLayoutToJson();


    void destroyAllTheObjects();
    void destroyObject(QString name = "");

    void updateWiredWithComboBox();

    void on_ComboBoxWiredWith_currentTextChanged(const QString &arg1);

protected:
    /**
     * @brief Событие закрытия окна.
     * @param event Событие закрытия.
     */
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
