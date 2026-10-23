#include "buoy.h"
#include "streamer.h" //костыль походу

Buoy::Buoy(QString Name, FixedItem *towingPoint, double angleToWired, double absoluteLayback,
           double AnthenaHeight, double towingDepth) :
    TowedItem(Name, towingPoint, angleToWired, absoluteLayback), AnthenaHeight(AnthenaHeight), towingDepth(towingDepth)
{
    this->itemType = "Buoy";
    if (towingPoint->itemType == "Streamer") {
        dynamic_cast<Streamer*>(towingPoint)->setEndBuoy(this);
    }
}

Buoy::Buoy() {}

Buoy::~Buoy()
{

    if (towingPoint->itemType == "Streamer") {
        dynamic_cast<Streamer*>(towingPoint)->setEndBuoy(nullptr);
    }
}
