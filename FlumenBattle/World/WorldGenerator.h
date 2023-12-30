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

    class WorldGenerator : public core::Singleton<WorldGenerator>
    {
        friend class WorldScene;

        void GenerateSociety(pregame::NewWorldData);

        std::atomic <bool> hasFinishedGenerating {false};

    public:
        int GenerateWorld(pregame::NewWorldData);

        int GetMaximumPolityCount(int) const;

        int GetMaximumSettlementCount(int) const;

        int GetMaximumGroupCount(int) const;

        bool HasFinishedGenerating() const {return hasFinishedGenerating;}

        void ResetGenerationProcess() const {hasFinishedGenerating = false;}
    };
}