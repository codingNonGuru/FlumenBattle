#pragma once

#include "FlumenBattle/Types.hpp"

namespace world::tile
{
    struct Ruin
    {
        Word Name;

        int FoundationDate;

        int ExtinctionDate;

        int Size;

        RaceTypes DominantRace;

        bool IsSettlementCenter;

        int RulerUniqueId;
    };
}