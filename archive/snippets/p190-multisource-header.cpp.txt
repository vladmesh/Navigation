// Archived from Parsers/p190_creator.cpp / p190_creator.h. Commit: 4ac56dd (Initial import), identical in main aee8ae8.
// Feature: P1/90 rows per source (filter by sourceID) and filling H0103 in the
// header; setter for the line name (lineName is still hard-coded in
// p190_creator.h, see audit H10).
// Status: ~10%. The sourceID filter was commented out and replaced by
// `if (true)`, fillSourceInfoInHeader has empty loops, nothing calls these.
//
// Declarations (class P190_creator):
// public:
//     QStringList createMainInfoBlock(int ffid, int sourceID);
//     void setLineName(const QString &newLineName);
// private:
//     void fillSourceInfoInHeader(QStringList& header); //not done

QStringList P190_creator::createMainInfoBlock(int ffid, int sourceID)
{
    QStringList res = {};
    // int sourceFlag = -1;
    // for (FixedItem* item: MyVault->ItemsVault) {
    //     if (item->itemType == "Source") {
    //         if (dynamic_cast<Source*>(item)->curSourceNum == sourceID) {
    //             sourceFlag = sourceID;
    //             break;
    //         }
    //     }
    // }
    // if (sourceFlag+1) {
    if (true) {
        QString tmp;
        // int pointNumber = ffid;
        for (FixedItem* item: MyVault->ItemsVault) {
            if (QString(item->metaObject()->className()) == "Streamer") {
                continue;
            }
            // if (item->itemType == "Source") {
            //     if (dynamic_cast<Source*>(item)->curSourceNum != sourceID) {
            //         continue;
            //     }
            // }
            tmp = createMainRow__new(item, ffid, -1);
            // if (QString(item->metaObject()->className()) == "Buoy") {
            //     currentBuoyNumber++;
            // }
            // currentSourceNumber++;
            if (tmp.isEmpty()) continue;
            res.append(tmp);
        }
    }
    return res;
}

void P190_creator::fillSourceInfoInHeader(QStringList &header)
{
    calcItemsCount();
    for (auto i: MyVault->ItemsVault) {

    }
    for (int i = 0; i < header.size(); i++) {
        if (header[i].startsWith("H0103") || header[i].startsWith("\"H0103")) {

        }
    }
}

void P190_creator::setLineName(const QString &newLineName)
{
    lineName = newLineName;
}
