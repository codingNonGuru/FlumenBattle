#pragma once

#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    class Settlement;

    struct PathSegment
    {
        WorldTile *To;

        WorldTile *From;

        RoadTypes Type;
    };

    struct Path
    {
        Settlement *To;

        Settlement *From;

        container::SmartBlock <WorldTile *, 16> Tiles;

        Path() {}

        Path(Settlement *to, Settlement *from) : To(to), From(from) {}

        void AddTile(WorldTile *tile) {*Tiles.Add() = tile;}
    };
}