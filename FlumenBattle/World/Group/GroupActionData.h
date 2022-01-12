#pragma once

namespace world
{
    class WorldTile;
}

namespace world::group
{
    union GroupActionData
    {
        WorldTile *TravelDestination;

        //GroupActionData() {}

        //GroupActionData(WorldTile *travelDestination) : TravelDestination(travelDestination) {}
    };
}