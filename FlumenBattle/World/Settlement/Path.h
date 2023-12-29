#pragma once

#include <utility>

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

        PathSegment() {}

        PathSegment(WorldTile *to, WorldTile *from) : To(to), From(from) {}

        PathSegment(WorldTile *to, WorldTile *from, RoadTypes type) : To(to), From(from), Type(type) {}

        /*bool operator== (const std::pair <WorldTile *, WorldTile *> tilePair) 
        {
            return ((To == tilePair.first && From == tilePair.second) || (To == tilePair.second && From == tilePair.first));
        }*/

        bool operator== (const PathSegment &segment) const
        {
            return ((To == segment.To && From == segment.From) || (To == segment.From && From == segment.To));
        }
    };

    struct Path
    {
        Settlement *To;

        Settlement *From;

        container::SmartBlock <WorldTile *, 16> Tiles;

        Path() {}

        Path(Settlement *to, Settlement *from) : To(to), From(from) {}

        void AddTile(WorldTile *tile);
    };
}