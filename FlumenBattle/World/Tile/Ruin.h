#pragma once

#include "FlumenBattle/Types.hpp"

namespace world::tile
{
    struct Ruin
    {
        int FoundationDate;

        int ExtinctionDate;

        int Size;

        RaceTypes DominantRace;

        bool IsSettlementCenter;
    };
}