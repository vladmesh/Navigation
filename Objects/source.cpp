#include "source.h"

Source::Source() {
    this->itemType = "Source";
}

Source::Source(QString Name, FixedItem *towingPoint, double angle, double wireLength,
               double AnthenaHeight, double sourceDepth)
    :TowedItem(Name, towingPoint, angle, wireLength), AnthenaHeight(AnthenaHeight), sourceDepth(sourceDepth)
{
    itemType = "Source";
}
