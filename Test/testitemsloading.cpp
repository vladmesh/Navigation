#include "testitemsloading.h"
#include "testdatapath.h"

testItemsLoading::testItemsLoading() {

    runTests();
}

void testItemsLoading::runTests()
{
    // itemsToJson();
    jsonToItems();
}

void testItemsLoading::itemsToJson() ///TODO::добавить в MW в Save configuration
{

    QString filepath = testDataPath("json/test.json");
    QVector<QPointer<FixedItem>> items = createItems2309();
    for (auto i:items) {
        qDebug() << i->name;
    }

    // ItemsLoader::setCreationPriority(items);
    // return;
    QJsonObject itemsJson = ItemsLoader::writeToJSON(items);
    ItemsLoader::saveJsonObjectToFile(itemsJson, filepath);
    // qDebug() << QJsonDocument(itemsJson).toJson(QJsonDocument::Indented);
}

void testItemsLoading::jsonToItems() ///TODO::добавить в MW в Save configuration
{
    QString filepath = testDataPath("json/test.json");
    QString filepathOut = testDataPath("json/test_test.json");
    QFile* file = new QFile(filepath);

    QVector<QPointer<FixedItem>> itemsfromJson = ItemsLoader::readFromJSON(file);

    // exit(123);

    qDebug() << itemsfromJson.size();
    QJsonObject itemsJsonForTest = ItemsLoader::writeToJSON(itemsfromJson);
    for (auto i:itemsJsonForTest ) {
        qDebug() << i;
    }
    ItemsLoader::saveJsonObjectToFile(itemsJsonForTest, filepathOut);
}


QVector<QPointer<FixedItem>> testItemsLoading::createItems2309()
{
    QVector<double> chans;
    for (int i = 0; i < 24; i++) {
        chans.append(i+1);
    }
    for (int i = 0; i < 24; i++) {
        chans.append(26 + i*2);
    }
    QVector<QPointer<FixedItem>> res;
    Fabric fb;
    FixedItem* shipBuoy = fb.CreateItem(FixedItemInfo(6.6,-5.4,0,"ship buoy"), false);
    shipBuoy->hasConnection = true;
    Connection_com* shipBuoyConnection = new Connection_com();
    shipBuoyConnection->filename = "ship buoy.nmea";
    shipBuoy->connection = shipBuoyConnection;


    FixedItem* StreamerTP = fb.CreateItem(FixedItemInfo(0.00,0,0,"StreamerTP"), false);
    StreamerTP->setItemForCalculations(shipBuoy);


    FixedItem* BoomerTP = fb.CreateItem(FixedItemInfo(0.45,-6.9,0,"BoomerTP"), false);
    BoomerTP->setItemForCalculations(shipBuoy);

    Source* boomer = fb.CreateItem(SourceInfo(TowedItemInfo(BoomerTP, 75.0, 0, "boomer")), false);
    Streamer* streamer = fb.CreateItem(StreamerInfo(TowedItemInfo(StreamerTP, 85.5, 0, "Streamer"), 48, chans));


    Buoy* endBuoy = fb.CreateItem(BuoyInfo(TowedItemInfo(streamer, 40.4, 0, "endbuoy"), 1, 0.5), false);
    endBuoy->hasConnection = true;

    streamer->setEndBuoy(endBuoy);
    Connection_com* conEnd = new Connection_com();
    conEnd->filename = "buoy_1.nmea";
    endBuoy->connection = conEnd;



    res.push_back(endBuoy );
    res.push_back(shipBuoy);
    res.push_back(StreamerTP);
    res.push_back(streamer);
    res.push_back(BoomerTP);
    res.push_back(boomer);

    return res;
}
