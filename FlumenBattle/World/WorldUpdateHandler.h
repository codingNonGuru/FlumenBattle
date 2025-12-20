#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Array.hpp"

namespace world
{
    namespace settlement
    {
        struct ExplorationRewardUpdateData;

        struct SimulationUpdateData;
    }

    class WorldUpdateHandler : public core::Singleton <WorldUpdateHandler>
    {
        container::Array <settlement::ExplorationRewardUpdateData> explorationRewardDatas;

        container::Array <settlement::SimulationUpdateData> simulationUpdateData;

    public:
        WorldUpdateHandler();

        void ResetAllData();

        void LoadSettlementSimulationUpdate(settlement::SimulationUpdateData);

        void LoadExplorationRewardData(settlement::ExplorationRewardUpdateData);

        void ProcessExplorationRewardData();

        void ProcessSimulationUpdateData();

        const container::Array <settlement::ExplorationRewardUpdateData> &GetExploreRewardDatas() const {return explorationRewardDatas;}
    };
}