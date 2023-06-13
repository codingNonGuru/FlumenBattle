#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

namespace world
{
    class Settlement;
    class WorldTile;

    struct SettlementBuildData
    {
        Word Name;

        WorldTile * Location;

        //RaceTypes Race;
    };

    class SettlementFactory
    {
    public:
        static Settlement* Create(SettlementBuildData); 
    };
}