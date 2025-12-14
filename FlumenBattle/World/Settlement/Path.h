#pragma once

#include <utility>

#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    namespace tile {struct WorldTile;}
}

namespace world::settlement
{
    class Settlement;

    struct PathSegment
    {
        tile::WorldTile *To;

        tile::WorldTile *From;

        RoadTypes Type;

        PathSegment() {}

        PathSegment(tile::WorldTile *to, tile::WorldTile *from) : To(to), From(from) {}

        PathSegment(tile::WorldTile *to, tile::WorldTile *from, RoadTypes type) : To(to), From(from), Type(type) {}

        /*bool operator== (const std::pair <WorldTile *, WorldTile *> tilePair) 
        {
            return ((To == tilePair.first && From == tilePair.second) || (To == tilePair.second && From == tilePair.first));
        }*/

        bool operator== (const PathSegment &segment) const
        {
            return ((To == segment.To && From == segment.From) || (To == segment.From && From == segment.To));
        }

        tile::WorldTile *GetOtherEnd(tile::WorldTile *thisEnd)
        {
            return thisEnd == To ? From : To;
        }
    };

    struct Path
    {
        Settlement *To;

        Settlement *From;

        container::SmartBlock <tile::WorldTile *, 16> Tiles;

        int Complexity;

        Path() {}

        Path(Settlement *to, Settlement *from) : To(to), From(from) {}

        bool operator== (const Path &path) const
        {
            return ((To == path.To && From == path.From) || (To == path.From && From == path.To));
        }

        void AddTile(tile::WorldTile *tile);

        Settlement *GetOther(const Settlement *settlement) 
        {
            if(To == settlement)
            {
                return From;
            }
            else
            {
                return To;
            }
        }

        const container::SmartBlock <tile::WorldTile *, 16> GetTilesTo(const Settlement *settlement) const;
    };
}