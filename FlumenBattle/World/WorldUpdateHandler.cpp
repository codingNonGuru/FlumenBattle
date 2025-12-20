#include "WorldUpdateHandler.h"
#include "FlumenBattle/World/Settlement/ExplorationHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity/Polity.h"

using namespace world;

#define MAXIMUM_DATA_PER_CONTAINER 64

WorldUpdateHandler::WorldUpdateHandler()
{
    explorationRewardDatas.Initialize(MAXIMUM_DATA_PER_CONTAINER);

    simulationUpdateData.Initialize(MAXIMUM_DATA_PER_CONTAINER);
}

void WorldUpdateHandler::ResetAllData()
{
    explorationRewardDatas.Reset();
}

void WorldUpdateHandler::LoadSettlementSimulationUpdate(settlement::SimulationUpdateData data)
{
    *simulationUpdateData.Add() = data;
}

void WorldUpdateHandler::LoadExplorationRewardData(settlement::ExplorationRewardUpdateData data)
{
    *explorationRewardDatas.Add() = data;
}

void WorldUpdateHandler::ProcessExplorationRewardData()
{
    for(auto &data : explorationRewardDatas)
    {
        if(data.Reward.Type == settlement::ExplorationRewards::POPULATION)
        {
            data.Settlement->IncreasePopulation();
        }
        else if(data.Reward.Type == settlement::ExplorationRewards::RESOURCE_CACHE)
        {
            data.Settlement->GetResource(data.Reward.Content.ResourceBatch.Type)->Storage += data.Reward.Content.ResourceBatch.Amount;
        }
        else if(data.Reward.Type == settlement::ExplorationRewards::TECH_BOOST)
        {
            data.Settlement->GetPolity()->AddTechnology(data.Reward.Content.Technology);
        }
    }
}

void WorldUpdateHandler::ProcessSimulationUpdateData()
{
    for(auto &data : simulationUpdateData)
    {
        data.Settlement->EnforceSimulationLevel();
    }

    simulationUpdateData.Reset();
}