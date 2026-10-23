#include <QtTest>
#include <QApplication>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "connectionlogic.h"
#include "itemsloader.h"
#include "postprocessor.h"

namespace {

// Connection абстрактный (reconnect), для теста calcQuality достаточно заглушки.
class StubConnection : public Connection
{
public:
    void reconnect() override {}
};

QJsonObject fixedItem(const QString &name, int priority)
{
    QJsonObject o;
    o["name"] = name;
    o["type"] = "Fixed";
    o["creationPriority"] = priority;
    o["x"] = 0.0;
    o["y"] = 0.0;
    o["z"] = 0.0;
    return o;
}

QJsonObject towedItem(const QString &name, const QString &towingPoint, int priority)
{
    QJsonObject towed;
    towed["Towing point"] = towingPoint;
    towed["absoluteLayback"] = 10.0;
    QJsonObject o;
    o["name"] = name;
    o["type"] = "Towed";
    o["creationPriority"] = priority;
    o["TowedInfo"] = towed;
    return o;
}

QString writeJson(const QTemporaryDir &dir, const QString &fileName, const QJsonObject &obj)
{
    QString path = dir.filePath(fileName);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return QString();
    f.write(QJsonDocument(obj).toJson());
    return path;
}

void deleteItems(QVector<QPointer<FixedItem>> &items)
{
    // Сначала буксируемые, затем точки буксировки: порядок обратный созданию.
    for (int i = items.size() - 1; i >= 0; --i)
        delete items[i].data();
    items.clear();
}

} // namespace

class TestCritical : public QObject
{
    Q_OBJECT

private slots:
    // C2
    void calcQualityRingBuffer();
    // C3
    void loaderValidLayoutSortedByPriority();
    void loaderSkipsItemWithMissingTowingPoint();
    void loaderConnectionWithoutEndpoint();
    void loaderInvalidJson();
    // C1
    void postProcessorNoFfidSource();
    void postProcessorTooFewFfid();
};

void TestCritical::calcQualityRingBuffer()
{
    // ~Connection зовёт file->close() без проверки на null (M5 в аудите,
    // вне этого PR), поэтому файл для записи NMEA создаём явно.
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    StubConnection con;
    con.create_file_for_nmea(dir.filePath("q.nmea"));
    // Заполняем окно (20) успешными пакетами: качество растёт до 1.
    float q = 0;
    for (int i = 0; i < 20; ++i)
        q = con.calcQuality(true);
    QVERIFY(qAbs(q - 1.0f) < 1e-4f);

    // Раньше 21-й пакет писался через end(). Прогоняем много кругов окна.
    for (int i = 0; i < 200; ++i)
        q = con.calcQuality(false);
    QVERIFY(qAbs(q) < 1e-4f);

    // Половина окна успешных: качество 0.5.
    for (int i = 0; i < 10; ++i)
        q = con.calcQuality(true);
    QVERIFY(qAbs(q - 0.5f) < 1e-4f);
}

void TestCritical::loaderValidLayoutSortedByPriority()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QJsonObject layout;
    // Ключи QJsonObject идут по алфавиту: "a_towed" раньше "b_ship",
    // правильный порядок даёт только creationPriority.
    layout["a_towed"] = towedItem("towed", "ship", 1);
    layout["b_ship"] = fixedItem("ship", 0);
    QFile file(writeJson(dir, "layout.json", layout));

    QStringList errors;
    auto items = ItemsLoader::readFromJSON(&file, &errors);
    QVERIFY2(errors.isEmpty(), qPrintable(errors.join("; ")));
    QCOMPARE(items.size(), 2);
    QCOMPARE(items[0]->name, QString("ship"));
    QCOMPARE(items[1]->name, QString("towed"));
    QCOMPARE(items[1]->creationPriority, 1);
    deleteItems(items);
}

void TestCritical::loaderSkipsItemWithMissingTowingPoint()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QJsonObject layout;
    layout["ship"] = fixedItem("ship", 0);
    layout["orphan"] = towedItem("orphan", "no_such_item", 1);
    QFile file(writeJson(dir, "layout.json", layout));

    QStringList errors;
    auto items = ItemsLoader::readFromJSON(&file, &errors);
    QCOMPARE(items.size(), 1);
    QCOMPARE(items[0]->name, QString("ship"));
    QCOMPARE(errors.size(), 1);
    QVERIFY(errors[0].contains("no_such_item"));
    deleteItems(items);
}

void TestCritical::loaderConnectionWithoutEndpoint()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QJsonObject ship = fixedItem("ship", 0);
    ship["hasConnection"] = true;
    ship["connection"] = QJsonObject{{"filename", "ship.nmea"}}; // ни IP, ни COMport
    QJsonObject layout;
    layout["ship"] = ship;
    QFile file(writeJson(dir, "layout.json", layout));

    QStringList errors;
    auto items = ItemsLoader::readFromJSON(&file, &errors);
    QCOMPARE(items.size(), 1);
    QVERIFY(!items[0]->hasConnection);
    QCOMPARE(errors.size(), 1);
    deleteItems(items);
}

void TestCritical::loaderInvalidJson()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("broken.json");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("[1, 2");
    f.close();

    QStringList errors;
    auto items = ItemsLoader::readFromJSON(&f, &errors);
    QVERIFY(items.isEmpty());
    QCOMPARE(errors.size(), 1);
}

// ItemsStorage::sortItemsByCalcPriority зацикливается на пустом хранилище
// (вне этого PR), поэтому в хранилище кладём один объект.
static FixedItem *oneItem(ItemsStorage &vault)
{
    Fabric fabric;
    FixedItem *item = fabric.CreateItem(FixedItemInfo(0, 0, 0, "ship"), false);
    vault.ItemsVault.push_back(item);
    return item;
}

void TestCritical::postProcessorNoFfidSource()
{
    ItemsStorage vault;
    std::unique_ptr<FixedItem> item(oneItem(vault));
    {
        PostProcessor pp;
        pp.setMyVault(&vault);
        QCOMPARE(pp.mainProcess(), int(PostProcessor::NoFfidSource));
        // pp ещё жив и разрушается штатно (раньше был delete this).
    }
}

void TestCritical::postProcessorTooFewFfid()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("ffid.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.close(); // пустой файл: ноль пар FFID-время

    ItemsStorage vault;
    std::unique_ptr<FixedItem> item(oneItem(vault));
    PostProcessor pp;
    pp.setMyVault(&vault);
    pp.ffidTimeSourceTxtFile = path;
    QCOMPARE(pp.mainProcess(), int(PostProcessor::NotEnoughFfid));
}

QTEST_MAIN(TestCritical)
#include "tst_critical.moc"
