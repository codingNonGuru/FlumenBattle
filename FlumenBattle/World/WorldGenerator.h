#pragma once

#include <atomic>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Grid.hpp"

namespace pregame
{
    struct NewWorldData;
}

namespace world
{
    class WorldScene;

    class WorldGenerator : public core::Singleton <WorldGenerator>
    {
        friend class WorldScene;

        void GenerateSociety(pregame::NewWorldData);

    public:
        int GenerateWorld(pregame::NewWorldData, const container::Grid <float> &, const container::Grid <float> &);

        int GetMaximumPolityCount(int) const;

        int GetMaximumSettlementCount(int) const;

        int GetMaximumGroupCount(int) const;
    };
}