#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    class Settlement;

    struct SettlementBuildData
    {
        world::WorldTile * Location;

        //RaceTypes Race;
    };

    class SettlementFactory
    {
    public:
        static Settlement* Create(SettlementBuildData); 
    };
}