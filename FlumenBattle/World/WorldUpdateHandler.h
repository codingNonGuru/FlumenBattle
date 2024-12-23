#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Array.hpp"

namespace world
{
    namespace settlement
    {
        struct ExplorationRewardUpdateData;
    }

    class WorldUpdateHandler : public core::Singleton <WorldUpdateHandler>
    {
        container::Array <settlement::ExplorationRewardUpdateData> explorationRewardDatas;

    public:
        WorldUpdateHandler();

        void ResetAllData();

        void LoadExplorationRewardData(settlement::ExplorationRewardUpdateData);

        void ProcessExplorationRewardData();

        const container::Array <settlement::ExplorationRewardUpdateData> &GetExploreRewardDatas() const {return explorationRewardDatas;}
    };
}