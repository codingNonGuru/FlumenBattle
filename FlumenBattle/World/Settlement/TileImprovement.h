#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    class WorldTile;
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

        int GetBonus(ResourceTypes, WorldTile *) const;
    };

    class TileImprovementFactory : public core::Singleton <TileImprovementFactory>
    {
    public:
        const TileImprovementType *BuildImprovementType(TileImprovements);
    };
}