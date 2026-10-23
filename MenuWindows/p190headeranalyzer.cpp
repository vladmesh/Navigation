#include "p190headeranalyzer.h"

p190HeaderAnalyzer::p190HeaderAnalyzer() {

}

bool p190HeaderAnalyzer::analizeVault()
{
    if (Vault == nullptr) return false;
    numOfBuoys = 0;
    numOfSources = 0;
    numOfStreamers = 0;

    for (QPointer<FixedItem> i: Vault->ItemsVault) {
        setIndexForSpecialItem(i);
    }
    qDebug() <<__FUNCTION__<< "numOfBuoys " << numOfBuoys << "numOfSources " << numOfSources << "numOfStreamers" << numOfStreamers;
    return true;
}

void p190HeaderAnalyzer::setIndexForSpecialItem(QPointer<FixedItem> item)
{
    if (item->itemType == "Fixed") {
        return;
    } else if (item->itemType == "Towed") {
        return;
    } else if (item->itemType == "Buoy") {
        Buoy *buoyRaw = qobject_cast<Buoy*>(item.data());
        if(buoyRaw) {
            // Если приведение прошло успешно, оборачиваем raw-указатель в QPointer<Buoy>
            QPointer<Buoy> buoy(buoyRaw);
            buoy->buoyID = numOfBuoys++;
        } else {
            qWarning() << __FUNCTION__ <<"Failed object cast Buoy";
        }
    } else if (item->itemType == "Streamer") {
        Streamer *StreamerRaw = qobject_cast<Streamer*>(item.data());
        if(StreamerRaw) {
            // Если приведение прошло успешно, оборачиваем raw-указатель в QPointer<Buoy>
            QPointer<Streamer> streamer(StreamerRaw);
            streamer->streamerID = numOfStreamers++;
        } else {
            qWarning() << __FUNCTION__ <<"Failed object cast Streamer";
        }
    } else if (item->itemType == "Source") {
        Source *sourceRaw = qobject_cast<Source*>(item.data());
        if(sourceRaw) {
            // Если приведение прошло успешно, оборачиваем raw-указатель в QPointer<Buoy>
            QPointer<Source> source(sourceRaw);
            source->sourceID = numOfSources++;
        } else {
            qWarning() << __FUNCTION__ <<"Failed object cast Source";
        }
    } else {
        return;
    }
}

QString p190HeaderAnalyzer::getHeaderStringFromItem(QPointer<FixedItem> item)
{
    QString res(80, ' ');

    if (QString(item->metaObject()->className()) == "Fixed") {
        return QString();
    } else if (QString(item->metaObject()->className()) == "Towed") {
        return QString();
    } else if (QString(item->metaObject()->className()) == "Buoy") {
        return QString();
    } else if (QString(item->metaObject()->className()) == "Streamer") {
        Streamer *StreamerRaw = qobject_cast<Streamer*>(item.data());
        if (StreamerRaw->streamerID == -1) return QString();
        res.replace(0, 5, "H0104");
        res.replace(68,1, QString::number(StreamerRaw->streamerID));
    } else if (QString(item->metaObject()->className()) == "Source") {
        Source *sourceRaw = qobject_cast<Source*>(item.data());
        if (sourceRaw->sourceID == -1) return QString();
        res.replace(0, 5, "H0103");
        res.replace(64, 1,QString::number(sourceRaw->sourceID));
    } else {
        return QString();
    }
    res.replace(33, item->name.length(), item->name);
    res.replace(60, 1, '1');
    return res;
}

void p190HeaderAnalyzer::setVault(ItemsStorage* newVault)
{
    Vault = newVault;
}
