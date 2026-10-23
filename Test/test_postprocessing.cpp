#include "test_postprocessing.h"
QVector<FixedItem *> testPostprocessing::createItems2309WithOnboard()
{
    QVector<double> chans;
    for (int i = 0; i < 24; i++) {
        chans.append(i+1);
    }
    for (int i = 0; i < 24; i++) {
        chans.append(26 + i*2);
    }
    QVector<FixedItem*> res;
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
    // Streamer* streamer = fb.CreateItem(StreamerInfo(TowedItemInfo(StreamerTP, 85.5, 0, "Streamer"), 48, chans));

    Streamer* streamer = new Streamer("Streamer", StreamerTP, 0, 85.5, 48, chans);
    // Buoy* endBuoy = fb.CreateItem(BuoyInfo(TowedItemInfo(streamer, 40.4, 0, "endbuoy"), 1, 0.5), false);

    Buoy* endBuoy = new Buoy("endbuoy", streamer, 0, 140.4, 1, 0.5);
    endBuoy->hasConnection = true;
    // endBuoy->setItemForCalculations(streamer);
    // shipBuoy->setItemForCalculations(endBuoy);

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

QVector<FixedItem *> testPostprocessing::createItemsСhainAlgo()
{
    QVector<FixedItem*> res;
    Fabric fb;


    FixedItem* shipBuoy = fb.CreateItem(FixedItemInfo(6.6,-5.4,0,"ship buoy"), false);
    shipBuoy->hasConnection = true;
    Connection_com* shipBuoyConnection = new Connection_com();
    shipBuoyConnection->filename = "boynextdoor.nmea";
    shipBuoy->connection = shipBuoyConnection;

    Streamer* streamer = new Streamer("Streamer", shipBuoy, 0, 1.1, 48, "1&2");
    Buoy* endBuoy = new Buoy("endbuoy", streamer, 0, 73.3, 1, 0.5);
    endBuoy->hasConnection = true;

    streamer->setEndBuoy(endBuoy);
    Connection_com* conEnd = new Connection_com();
    conEnd->filename = "dangeonmuster";
    endBuoy->connection = conEnd;



    res.push_back(shipBuoy);
    res.push_back(endBuoy );
    res.push_back(streamer);

    return res;
}

void testPostprocessing::testChainAlgo()
{
    QVector<FixedItem*> items;
    items = createItemsСhainAlgo();
    PostProcessor pr;
    ItemsStorage* Vault = new ItemsStorage;
    for (auto i: items) {
        pr.items.push_back(i);
        // if (i->itemType == "Streamer") {
        //     dynamic_cast<Streamer*>(i)->setWireLength(dynamic_cast<Streamer*>(i)->wireLength + 50);
        // }
        Vault->ItemsVault.push_back(i);
    }
    Vault->sortItemsByCalcPriority();
    Vault->setItemsCreationIndex(Vault->ItemsVault);
    Vault->setItemForCalculation();
    pr.p190Creator->setMyVault(Vault);
    setPath(pr.p190Creator);

    QString ndir = "C:/Users/sabrahar/Desktop/FINAL";
    QDir nmeadir(ndir);

    pr.pairs = readFileAndGeneratePairs("C:/Users/sabrahar/Desktop/AZERTEST/ChainAlgo_geom.txt");
    // for (auto i: pr.pairs) {
    //     qDebug() << i.ffid << i.julianDay << i.time;
    // }

    NmeaParser parser;

    int posbouy = 0;
    int posSTP = 0;

    QFile* vesBuoyFile = new QFile("C:/Users/sabrahar/Desktop/AZERTEST/lead_object.nmea");
    if (!vesBuoyFile->isOpen()) vesBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);

    QFile* endBuoyFile = new QFile("C:/Users/sabrahar/Desktop/AZERTEST/towed_object.nmea");
    if (!endBuoyFile->isOpen()) endBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);


    for (SegYReader::SegyData pair: pr.pairs) {
        NmeaParser::NmeaRMCData RMCSTP;
        Q_UNUSED(RMCSTP); //это чтобы анализатор кода не ругался и не выдавал варнинги
        QStringList res_endbuoy = pr.findNmeaForSegy(pair, endBuoyFile, &posbouy);
        QStringList res_vesbuoy = pr.findNmeaForSegy(pair, vesBuoyFile, &posSTP);
        NmeaParser::CoordinateData GGAendbuoy_first = parser.parseNmeaGGA(res_endbuoy[0]);
        NmeaParser::CoordinateData GGAvesBuoy_first = parser.parseNmeaGGA(res_vesbuoy[0]);
        NmeaParser::CoordinateData GGAendbuoy_second = parser.parseNmeaGGA(res_endbuoy[1]);
        NmeaParser::CoordinateData GGAvesBuoy_second= parser.parseNmeaGGA(res_vesbuoy[1]);

        if (GGAendbuoy_first.dateTime.time().secsTo(pair.time) > 10) {
            qDebug() << items[0]->name << "connection false";
            qDebug() << pair.time;
        }
        NmeaParser::CoordinateData GGAendbuoyTrue = pr.calcTruePosition(GGAendbuoy_first,GGAendbuoy_second,pair.time, GGAendbuoy_first.dateTime.time(),GGAendbuoy_second.dateTime.time());
        NmeaParser::CoordinateData GGAvesBuoyTrue = pr.calcTruePosition(GGAvesBuoy_first,GGAvesBuoy_second,pair.time, GGAvesBuoy_first.dateTime.time(),GGAvesBuoy_second.dateTime.time());


        items[0]->lastGGAData = GGAvesBuoyTrue; //shipBuoy
        items[1]->lastGGAData = GGAendbuoyTrue; //endBuoy

        for (auto i: items) {
            i->calcItemCoordinates();
        }


        // if (items[1]->showAzDistToObj(items[2])){
        //     items[1]->showAzDistToObj(items[4]);
        //     items[2]->showAzDistToObj(items[4]);
        //     items[1]->showAzDistToObj(items[0]);
        //     items[2]->showAzDistToObj(items[3]);
        //     items[4]->showAzDistToObj(items[5]);
        //     items[2]->showAzDistToObj(items[0]);
        //     items[1]->showAzDistToObj(items[3]);
        //     items[5]->showAzDistToObj(items[3]);
        //     qDebug() << qRadiansToDegrees( items[2]->azimuthOfMovement) << "movement";
        //     qDebug() << qRadiansToDegrees(items[3]->realAzimuthOfTowingRadians)<< "towing\n";

        //     // qDebug() << pair.ffid << pair.time;
        //     // qDebug() <<"END"+res_endbuoy[0]<<"\nVES"+res_vesbuoy[0];
        // }
        pr.p190Creator->createP190File(pair.julianDay);
        pr.p190Creator->setDayOfSurvey(pair.julianDay);
        pr.p190Creator->setFFID(pair.ffid);
        pr.p190Creator->createShotBlock();

    }
    // for (QString i:perpendiculars) {
    //     perpendicularsFile.write(i.toStdString().c_str());
    // }
    // delete endBuoyFile;
    // return 1;

}

QVector<FixedItem *> testPostprocessing::createItems2309EndBuoyOnlyFans()
{
    QVector<double> chans;
    for (int i = 0; i < 24; i++) {
        chans.append(i+1);
    }
    for (int i = 0; i < 24; i++) {
        chans.append(26 + i*2);
    }
    QVector<FixedItem*> res;
    Fabric fb;
    FixedItem* shipBuoy = fb.CreateItem(FixedItemInfo(6.6,-5.4,0,"ship buoy"), false);


    FixedItem* StreamerTP = fb.CreateItem(FixedItemInfo(0.00,0,0,"StreamerTP"), false);


    FixedItem* BoomerTP = fb.CreateItem(FixedItemInfo(0.45,-6.9,0,"BoomerTP"), false);

    Source* boomer = fb.CreateItem(SourceInfo(TowedItemInfo(BoomerTP, 75, 0, "boomer")), false);
    Streamer* streamer = fb.CreateItem(StreamerInfo(TowedItemInfo(StreamerTP, 85.5, 0, "streamer"), 48, chans));

    Buoy* endBuoy = fb.CreateItem(BuoyInfo(TowedItemInfo(streamer, 40.4, 0, "endbuoy"), 1, 0.5), false);
    endBuoy->hasConnection = true;
    shipBuoy->setItemForCalculations(endBuoy);


    streamer->setEndBuoy(endBuoy);
    StreamerTP->setItemForCalculations(endBuoy);
    BoomerTP->setItemForCalculations(endBuoy);

    Connection_com* conEnd = new Connection_com();
    conEnd->filename = "buoy_1.nmea";
    endBuoy->connection = conEnd;



    res.push_back(endBuoy );
    res.push_back(shipBuoy);
    res.push_back(StreamerTP);
    res.push_back(streamer);
    res.push_back(BoomerTP);
    res.push_back(boomer);
    // res.push_back(leadBuoy);

    // for (auto i: res) {
    //     qDebug() << i->metaObject()->className();
    // }

    return res;
}


QVector<FixedItem*> testPostprocessing::createItems2209()
{
    QVector<double> chans;
    for (int i = 0; i < 24; i++) {
        chans.append(i+1);
    }
    for (int i = 0; i < 24; i++) {
        chans.append(26 + i*2);
    }
    QVector<FixedItem*> res;
    Fabric fb;
    FixedItem* shipBuoy = fb.CreateItem(FixedItemInfo(6.6,-5.4,0,"ship buoy"), false);
    shipBuoy->hasConnection = true;
    Connection_com* shipBuoyConnection = new Connection_com();
    shipBuoyConnection->filename = "ship buoy.nmea";
    shipBuoy->connection = shipBuoyConnection;


    FixedItem* StreamerTP = fb.CreateItem(FixedItemInfo(0.00,0,0,"StreamerTP"), false);
    StreamerTP->setItemForCalculations(shipBuoy);


    FixedItem* BoomerTP = fb.CreateItem(FixedItemInfo(0.5,-6.9,0,"BoomerTP"), false);
    BoomerTP->setItemForCalculations(shipBuoy);

    Source* boomer = fb.CreateItem(SourceInfo(TowedItemInfo(BoomerTP, 75, 0, "boomer")), false);
    // Streamer* streamer = fb.CreateItem(StreamerInfo(TowedItemInfo(StreamerTP, 85.5, 0, "streamer"), 48, chans));

    Streamer* streamer = new Streamer("streamer", BoomerTP, 0, 85.5, 48, chans);

    Buoy* endBuoy = fb.CreateItem(BuoyInfo(TowedItemInfo(streamer, 30, 0, "endbuoy"), 1, 0.5), false);
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
    // res.push_back(leadBuoy);

    // for (auto i: res) {
    //     qDebug() << i->metaObject()->className();
    // }

    return res;

}




testPostprocessing::testPostprocessing() {
    // testSearchingNmea22();
    // testSearchingNmea23WithOnboard();
    // testSearchingNmea23EndBuoyOnlyFans();
    // testPpkFindString();
    testChainAlgo();
}




int testPostprocessing::testSearchingNmea23WithOnboard()
{
    // Logger::instance();
    QVector<FixedItem*> items;
    items = createItems2309WithOnboard();
    // for (auto i: items) {
    //     qDebug() << i->name;
    // }
    // exit(123);
    PostProcessor pr;
    ItemsStorage* Vault = new ItemsStorage;

    QStringList perpendiculars;
    QFile perpendicularsFile("./perpendiculars.txt");
    perpendicularsFile.open(QIODevice::WriteOnly);


    Streamer* strm;

    for (auto i: items) {
        pr.items.push_back(i);
        Vault->ItemsVault.push_back(i);
        // qDebug() << i->name;
    }
    pr.p190Creator->setMyVault(Vault);

    QString sdir = "C:/Users/sabrahar/Desktop/FINAL/20240923_UHRS_48/RAW";
    QDir segydir(sdir);
    QString ndir = "C:/Users/sabrahar/Desktop/FINAL";
    QDir nmeadir(ndir);

    pr.pairs = readFileAndGeneratePairs("C:/Users/sabrahar/Desktop/FINAL/240923_geom.txt");

    NmeaParser parser;

    int posbouy = 0;
    int posSTP = 0;

    QFile* endBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/23092024endBuoy.nmea");
    if (!endBuoyFile->isOpen()) endBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);

    QFile* vesBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/23092024OnboardBuoy.nmea");
    if (!vesBuoyFile->isOpen()) vesBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);


    for (SegYReader::SegyData pair: pr.pairs) {

        // qDebug() << pair.dateTime;
        // continue;
        NmeaParser::NmeaRMCData RMCSTP;

        QStringList res_endbuoy = pr.findNmeaForSegy(pair, endBuoyFile, &posbouy);
        QStringList res_vesbuoy = pr.findNmeaForSegy(pair, vesBuoyFile, &posSTP);
        NmeaParser::CoordinateData GGAendbuoy_first = parser.parseNmeaGGA(res_endbuoy[0]);
        NmeaParser::CoordinateData GGAvesBuoy_first = parser.parseNmeaGGA(res_vesbuoy[0]);
        NmeaParser::CoordinateData GGAendbuoy_second = parser.parseNmeaGGA(res_endbuoy[1]);
        NmeaParser::CoordinateData GGAvesBuoy_second= parser.parseNmeaGGA(res_vesbuoy[1]);
        if (res_vesbuoy.size() == 3) {
            RMCSTP= parser.parseNmeaRMC(res_vesbuoy[2]);
        }


        if (GGAendbuoy_first.dateTime.time().secsTo(pair.time) > 10) {
            qDebug() << items[0]->name << "connection false";
            qDebug() << pair.time;
        }
        NmeaParser::CoordinateData GGAendbuoyTrue = pr.calcTruePosition(GGAendbuoy_first,GGAendbuoy_second,pair.time, GGAendbuoy_first.dateTime.time(),GGAendbuoy_second.dateTime.time());
        NmeaParser::CoordinateData GGAvesBuoyTrue = pr.calcTruePosition(GGAvesBuoy_first,GGAvesBuoy_second,pair.time, GGAvesBuoy_first.dateTime.time(),GGAvesBuoy_second.dateTime.time());


        items[0]->lastGGAData = GGAendbuoyTrue; //endBuoy
        items[1]->lastGGAData = GGAvesBuoyTrue; //shipBuoy

        for (auto i: items) {
            i->calcItemCoordinates();
            if (QString(i->metaObject()->className()) == "Streamer") {
                strm = dynamic_cast<Streamer*>(i);
                if (pair.ffid / 100 == 0) {
                }
            }
        }

        double dist = shortestDistanceToLine(QPointF(items[5]->x_coor, items[5]->y_coor), QPointF(strm->getChan(1)->x_coor,strm->getChan(1)->y_coor),
                                            QPointF(strm->getChan(48)->x_coor,strm->getChan(48)->y_coor));
        // dist = shortestDistanceToLine(QPointF(items[1]->x_coor, items[1]->y_coor), QPointF(items[2]->x_coor, items[2]->y_coor),
        //                               QPointF(items[4]->x_coor, items[4]->y_coor));
        // dist = pow((items[5]->x_coor - strm->x_coor)*(items[5]->x_coor - strm->x_coor) + (items[5]->y_coor - strm->y_coor)*(items[5]->y_coor - strm->y_coor), 0.5);

        if (pair.ffid > 0) {
            perpendiculars.append(QString(pair.time.toString() + "\t" + QString::number(dist)).replace('.', ',') + "\n");
        }

        if (items[1]->showAzDistToObj(items[2])){
            items[1]->showAzDistToObj(items[4]);
            items[2]->showAzDistToObj(items[4]);
            items[1]->showAzDistToObj(items[0]);
            items[2]->showAzDistToObj(items[3]);
            items[4]->showAzDistToObj(items[5]);
            items[2]->showAzDistToObj(items[0]);
            items[1]->showAzDistToObj(items[3]);
            items[5]->showAzDistToObj(items[3]);
            qDebug() << qRadiansToDegrees( items[2]->azimuthOfMovement) << "movement";
            qDebug() << qRadiansToDegrees(items[3]->realAzimuthOfTowingRadians)<< "towing\n";

            // qDebug() << pair.ffid << pair.time;
            // qDebug() <<"END"+res_endbuoy[0]<<"\nVES"+res_vesbuoy[0];
        }
        pr.p190Creator->createP190File(pair.julianDay);
        pr.p190Creator->setDayOfSurvey(pair.julianDay);
        pr.p190Creator->setFFID(pair.ffid);
        pr.p190Creator->createShotBlock();
        // break;
        // NmeaParser::NmeaGGAData GGAData = pr
    }
    for (QString i:perpendiculars) {
        perpendicularsFile.write(i.toStdString().c_str());
    }
    // delete endBuoyFile;
    return 1;
}
void testPostprocessing::testPpkFindString()
{
    int i = 1;
    PostProcessor pr;
    auto pairs = readFileAndGeneratePairs("C:/Users/sabrahar/Desktop/FINAL/240922_geom.txt");
    QFile* ppkFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/test/GD_buoy_PPK_20240922043218.ppk");
    // ppkFile->open(QIODevice::ReadOnly);
    for (auto pair: pairs) {
        QStringList parts = pr.findPpkForSegy(pair, ppkFile, &i);
        NmeaParser::CoordinateData first = PpkParser::parseLine(parts[0]);
        NmeaParser::CoordinateData second = PpkParser::parseLine(parts[1]);
        NmeaParser::CoordinateData trueCoor = pr.calcTruePosition(first, second, pair.time, first.dateTime.time(), second.dateTime.time());

        // qDebug() << pair.ffid << pair.time;
        // break;
    }
}


int testPostprocessing::testSearchingNmea22()
{
    // Logger::instance();
    QVector<FixedItem*> items = createItems2209();
    PostProcessor pr;
    ItemsStorage* Vault = new ItemsStorage;

    Streamer* strm;

    QStringList perpendiculars;
    QFile perpendicularsFile("./perpendiculars.txt");
    perpendicularsFile.open(QIODevice::WriteOnly);

    for (auto i: items) {
        pr.items.push_back(i);
        Vault->ItemsVault.push_back(i);
    }
    pr.p190Creator->setMyVault(Vault);

    // QString ndir = "C:/Users/sabrahar/Desktop/FINAL";
    // QDir nmeadir(ndir);

    pr.p190Creator->createP190File();
    pr.pairs = readFileAndGeneratePairs("C:/Users/sabrahar/Desktop/FINAL/240922_geom.txt");

    NmeaParser parser;

    int posbouy = 0;
    int posSTP = 0;
    double dist;
    QFile* endBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/22092024endBuoy.nmea");
    if (!endBuoyFile->isOpen()) endBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);

    QFile* vesBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/22092024OnboardBuoy.nmea");
    if (!vesBuoyFile->isOpen()) vesBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);

    NmeaParser::CoordinateData GGAvesBuoyPrev = {{0,0}, {0,0},QDateTime(), 1};

    for (SegYReader::SegyData pair: pr.pairs) {
        // NmeaParser::NmeaRMCData RMCSTP;


        QStringList res_endbuoy = pr.findNmeaForSegy(pair, endBuoyFile, &posbouy);
        QStringList res_vesbuoy = pr.findNmeaForSegy(pair, vesBuoyFile, &posSTP);
        NmeaParser::CoordinateData GGAendbuoy_first = parser.parseNmeaGGA(res_endbuoy[0]);
        NmeaParser::CoordinateData GGAvesBuoy_first = parser.parseNmeaGGA(res_vesbuoy[0]);
        NmeaParser::CoordinateData GGAendbuoy_second = parser.parseNmeaGGA(res_endbuoy[1]);
        NmeaParser::CoordinateData GGAvesBuoy_second= parser.parseNmeaGGA(res_vesbuoy[1]);
        // if (res_vesbuoy.size() == 3) {
        //     RMCSTP= parser.parseNmeaRMC(res_vesbuoy[2]);
        // }


        // qDebug() << GGAbuoy_first.dateTime.time() << GGASTP_first.dateTime.time();
        NmeaParser::CoordinateData GGAendbuoyTrue = pr.calcTruePosition(GGAendbuoy_first,GGAendbuoy_second,pair.time, GGAendbuoy_first.dateTime.time(),GGAendbuoy_second.dateTime.time());
        NmeaParser::CoordinateData GGAvesBuoyTrue = pr.calcTruePosition(GGAvesBuoy_first,GGAvesBuoy_second,pair.time, GGAvesBuoy_first.dateTime.time(),GGAvesBuoy_second.dateTime.time());

        // qDebug() << GGAvesBuoyTrue.coordinate << GGAvesBuoyTrue.coorUTM;

        //checking distance
        dist = getDistance(GGAvesBuoyPrev, GGAvesBuoyTrue);
        if (dist>=0 && dist < 0.5){
            // qDebug() << dist << pair.ffid << pair.time;
            // qDebug() << res_endbuoy[0] << "\n" << res_endbuoy[1];
            // qDebug() << res_vesbuoy[0] << "\n" << res_vesbuoy[1];
            // qDebug() << GGAvesBuoyPrev.coorUTM << GGAvesBuoyTrue.coorUTM;
            // qDebug() << "__________________________";
        } else {
            // qDebug() << dist << " IS GOOD";
        }
        GGAvesBuoyPrev.coordinate = QGeoCoordinate(GGAvesBuoyTrue.coordinate);
        GGAvesBuoyPrev.coorUTM = QPointF(GGAvesBuoyTrue.coorUTM);
        // qDebug() << GGAvesBuoyPrev.coordinate;



        items[0]->lastGGAData = GGAendbuoyTrue; //endBuoy
        items[1]->lastGGAData = GGAvesBuoyTrue; //shipBuoy



        for (auto i: items) {
            i->calcItemCoordinates();
            if (QString(i->metaObject()->className()) == "Streamer") {
                strm = dynamic_cast<Streamer*>(i);
                if (pair.ffid % 100 == 0) {
                    // Streamer* strm = dynamic_cast<Streamer*>(i);
                    // qDebug() << "FAKE AZIMUTH"<<GGAendbuoyTrue.coordinate.azimuthTo(GGAvesBuoyTrue.coordinate);
                    // qDebug() << "REAL AZIMUTH OF TOWING" << qRadiansToDegrees(strm->realAzimuthOfTowingRadians);
                    // qDebug() <<"FFID" << pair.ffid;
                }
            }
        }
        double dist = shortestDistanceToLine(QPointF(items[5]->x_coor, items[5]->y_coor), QPointF(strm->x_coor, strm->y_coor),
                                            QPointF(strm->getChan(48)->x_coor,strm->getChan(48)->y_coor));

        if (pair.ffid > 0) {
            perpendiculars.append(QString(pair.time.toString() + "\t" + QString::number(dist)).replace('.', ',') + "\n");
        }

        if (items[1]->showAzDistToObj(items[2])){
            items[1]->showAzDistToObj(items[4]);
            items[2]->showAzDistToObj(items[4]);
            items[1]->showAzDistToObj(items[0]);
            items[2]->showAzDistToObj(items[3]);
            items[4]->showAzDistToObj(items[5]);
            items[2]->showAzDistToObj(items[0]);
            items[1]->showAzDistToObj(items[3]);
            items[5]->showAzDistToObj(items[3]);
            qDebug() << qRadiansToDegrees( items[2]->azimuthOfMovement) << "movement";
            qDebug() << qRadiansToDegrees(items[3]->realAzimuthOfTowingRadians)<< "towing\n";

            // qDebug() << pair.ffid << pair.time;
            // qDebug() <<"END"+res_endbuoy[0]<<"\nVES"+res_vesbuoy[0];
        }


        pr.p190Creator->createP190File(pair.julianDay);
        pr.p190Creator->setDayOfSurvey(pair.julianDay);
        pr.p190Creator->setFFID(pair.ffid);
        pr.p190Creator->createShotBlock();
        // break;
        // NmeaParser::NmeaGGAData GGAData = pr
    }
    for (QString i:perpendiculars) {
        perpendicularsFile.write(i.toStdString().c_str());
    }


    delete endBuoyFile;
    return 1;
}



int testPostprocessing::testSearchingNmea23EndBuoyOnlyFans()
{
    NmeaParser::CoordinateData shipBuoylast;
    NmeaParser::CoordinateData StreamerTPLast;
    NmeaParser::CoordinateData BoomerTPLast;
    Streamer* strm;

    // Logger::instance();
    QVector<FixedItem*> items = createItems2309EndBuoyOnlyFans();
    PostProcessor pr;
    ItemsStorage* Vault = new ItemsStorage;

    for (auto i: items) {
        pr.items.push_back(i);
        Vault->ItemsVault.push_back(i);
    }
    pr.p190Creator->setMyVault(Vault);

    QString ndir = "C:/Users/sabrahar/Desktop/FINAL";
    QDir nmeadir(ndir);

    pr.p190Creator->createP190File();
    pr.pairs = readFileAndGeneratePairs("C:/Users/sabrahar/Desktop/FINAL/20240923_geom_without_onboard.txt");

    NmeaParser parser;

    int posbouy = 0;
    // int posSTP = 0;

    QFile* endBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/23092024endBuoy.nmea");
    if (!endBuoyFile->isOpen()) endBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);

    // QFile* vesBuoyFile = new QFile("C:/Users/sabrahar/Desktop/FINAL/PPK GNSS data/23092024OnboardBuoy.nmea");
    // if (!vesBuoyFile->isOpen()) vesBuoyFile->open(QIODevice::ReadOnly | QIODevice::Text);


    for (SegYReader::SegyData pair: pr.pairs) {

        QStringList res_endbuoy = pr.findNmeaForSegy(pair, endBuoyFile, &posbouy);
        NmeaParser::CoordinateData GGAendbuoy_first = parser.parseNmeaGGA(res_endbuoy[0]);
        NmeaParser::CoordinateData GGAendbuoy_second = parser.parseNmeaGGA(res_endbuoy[1]);

        NmeaParser::CoordinateData GGAendbuoyTrue = pr.calcTruePosition(GGAendbuoy_first,GGAendbuoy_second, pair.time, GGAendbuoy_first.dateTime.time(),GGAendbuoy_second.dateTime.time());




        items[0]->lastGGAData = GGAendbuoyTrue; //endBuoy

        for (auto i: items) {
            i->calcItemCoordinates();
            if (QString(i->metaObject()->className()) == "Streamer") {
                strm = dynamic_cast<Streamer*>(i);
                if (pair.ffid / 100 == 0 || true) {
                    // qDebug() << "REAL AZIMUTH OF TOWING" << qRadiansToDegrees(strm->realAzimuthOfTowingRadians);
                    // qDebug() <<"FFID" << pair.ffid;
                }
            }
            if (i->name == "shipBuoy") {
                shipBuoylast = i->lastGGAData;
                // qDebug() << shipBuoylast.coordinate.distanceTo(GGAendbuoyTrue.coordinate);
                // qDebug() << shipBuoylast.coordinate.azimuthTo(GGAendbuoyTrue.coordinate);
                // qDebug() <<i->name;
                // qDebug() <<
            } else if (i->name == "BoomerTP") {
                BoomerTPLast = i->lastGGAData;
            } else if (i->name == "StreamerTP") {
                StreamerTPLast = i->lastGGAData;
            }

        }

        qDebug() << shortestDistanceToLine(QPointF(items[5]->x_coor, items[5]->y_coor), QPointF(strm->x_coor, strm->y_coor),
                                   QPointF(strm->getChan(48)->x_coor,strm->getChan(48)->y_coor));
        qDebug() <<"distance";


        pr.p190Creator->setFFID(pair.ffid);
        pr.p190Creator->createShotBlock();
        // break;
        // NmeaParser::NmeaGGAData GGAData = pr
    }
    delete endBuoyFile;
    return 1;
}







QVector<SegYReader::SegyData> testPostprocessing::readFileAndGeneratePairs(const QString& fileName) {
    QVector<SegYReader::SegyData> pairs;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Cannot open file!" << std::endl;
        return pairs;
    }

    QTextStream in(&file);
    // Пропускаем первую строку с заголовками
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split('\t');  // Разделитель - табуляция

        if (fields.size() == 6) {
            int ffid = fields[0].toInt();
            int julianDay = fields[1].toInt();
            // qDebug() <<julianDay << __LINE__;
            int hour = fields[2].toInt();
            int minute = fields[3].toInt();
            int second = fields[4].toInt();
            int microsecond = fields[5].split('.').first().toInt();  // Отбрасываем дробную часть
            // QDate day = getDateFromDayOfYear(julianDay);

            QTime time = QTime(hour, minute, second, microsecond / 1000);  // Микросекунды преобразуем в миллисекунды

            SegYReader::SegyData pair(ffid, time, julianDay);
            pairs.push_back(pair);
            // qDebug() << pairs.at(pairs.size() - 1).dateTime; //<< __FUNCTION__
        }
    }
    // for (auto i: pairs) {
    //     qDebug() << i.time << i.julianDay << __LINE__;
    // }
    file.close();
    return pairs;
}

double testPostprocessing::shortestDistanceToLine(const QPointF &point, const QPointF &lineStart, const QPointF &lineEnd)
{
    QPointF lineVector = lineEnd - lineStart;

    // Вектор от начала прямой до заданной точки
    QPointF pointVector = point - lineStart;

    // Вычисление скалярного произведения
    double t = QPointF::dotProduct(pointVector, lineVector) / (qPow(lineVector.x(), 2) + qPow(lineVector.y(), 2));

    // Вычисление ближайшей точки на прямой
    QPointF projection = lineStart + t * lineVector;

    // Возвращаем расстояние от точки до проекции (ближайшей точки на прямой)
    return qSqrt(qPow(point.x() - projection.x(), 2) + qPow(point.y() - projection.y(), 2));
}

void testPostprocessing::setPath(P190_creator* victim)
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
    victim->setPath(dir);
}


double testPostprocessing::getDistance(NmeaParser::CoordinateData prev, NmeaParser::CoordinateData next)
{
    if (prev.coordinate.isValid()) {
        return prev.coordinate.distanceTo(next.coordinate);
    } else {
        qDebug() << prev.coordinate;
        return -1;
    }
}

