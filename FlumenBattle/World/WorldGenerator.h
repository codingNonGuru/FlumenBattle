#pragma once

#include <atomic>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Grid.hpp"

#define RACE_MAP_COUNT 2

namespace pregame
{
    struct NewWorldData;
    struct MemberData;
}

namespace world
{
    class WorldScene;

    class WorldGenerator : public core::Singleton <WorldGenerator>
    {
        friend class WorldScene;

        container::Grid <float> raceDistributionMaps[RACE_MAP_COUNT];

        void GenerateSociety(pregame::NewWorldData);

        void DefineRivers();

    public:
        int GenerateWorld(pregame::NewWorldData, const container::Grid <float> &, const container::Grid <float> &, const container::Grid <float> &, const container::Grid <float> &);

        int GetMaximumPolityCount(int) const;

        int GetMaximumSettlementCount(int) const;

        int GetMaximumGroupCount(int) const;

        container::Grid <float> *GetRaceDistributionMaps() {return raceDistributionMaps;}

        void GeneratePlayerGroup(const container::Array <pregame::MemberData> &);

        void FinishGeneration();
    };
}