#pragma once

#include <atomic>

#include "FlumenCore/Singleton.h"

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
        int GenerateWorld(pregame::NewWorldData);

        int GetMaximumPolityCount(int) const;

        int GetMaximumSettlementCount(int) const;

        int GetMaximumGroupCount(int) const;
    };
}