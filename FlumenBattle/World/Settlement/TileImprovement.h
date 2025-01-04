#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    namespace tile {struct WorldTile;}
}

namespace world::settlement
{
    struct TileImprovementType
    {
        TileImprovements Type;

        ResourceTypes Resource;

        Word Name;

        Word TextureName;

        int Cost;

        int GetBonus(ResourceTypes, tile::WorldTile *) const;
    };

    class TileImprovementFactory : public core::Singleton <TileImprovementFactory>
    {
    public:
        const TileImprovementType *BuildImprovementType(TileImprovements);
    };
}