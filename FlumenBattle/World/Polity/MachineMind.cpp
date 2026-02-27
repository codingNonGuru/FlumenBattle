#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::polity;

void MachineMind::MakeDecision(Polity &polity) const
{
    UpdateWorkforce(polity);

    for(auto &settlement : polity.GetSettlements())
    {
        if(settlement->GetBuildingProduction()->Is(settlement::ProductionOptions::NONE) == true)
        {
            settlement->DecideProduction(settlement::ProductionClasses::BUILDING);
        }

        if(settlement->GetGroupProduction()->Is(settlement::ProductionOptions::NONE) == true)
        {
            settlement->DecideProduction(settlement::ProductionClasses::RECRUITMENT);
        }
    }
}

void MachineMind::DecideResearch(Polity &polity) const
{
    auto &techRoster = polity.technologyRoster;

    if(techRoster->IsResearchingAnything())
        return;

    struct Priority
    {
        science::Technologies Technology;

        int Level;
    };

    static const container::Array <Priority> priorities = {
        {science::Technologies::MASONRY, 0}, 
        {science::Technologies::HAND_WASHING, 1}, 
        {science::Technologies::TRAINED_SENTINELS, 2}};

    for(int i = 0; i < 3; ++i)
    {
        for(auto priority = priorities.GetStart(); priority != priorities.GetEnd(); ++priority)
        {
            if(priority->Level == i && techRoster->HasDiscovered(priority->Technology) == false)
            {
                techRoster->StartResearching(priority->Technology);
                
                return;
            }
        }
    }
}

void MachineMind::UpdateWorkforce(Polity &polity) const
{
    for(auto &settlement : polity.GetSettlements())
    {
        UpdateWorkforce(*settlement);
    }
}

static container::Pool <WorkInstruction> workInstructions(128);

static int workPriority = 0;

static bool hasWorkforceChanged = false;

struct TileScore
{
    world::settlement::SettlementTile *Tile;

    int GeneralScore;
};

struct JobScore
{
    world::settlement::Job *Job;

    int Score;
};

struct ResourceScore
{
    world::settlement::Resource *Resource;

    int Score;
};

void MachineMind::UpdateWorkforce(settlement::Settlement &settlement) const
{
    if(settlement.IsDeepSettlement() == true)
    {
        workInstructions.Reset();

        workPriority = 0;

        hasWorkforceChanged = false;

        UpdateWorkforceBasedOnCentralPlan(settlement);

        UpdateIntermediateResourceWorkforce(settlement);

        UpdateWorkforceBasedOnFreeMarket(settlement);

        if(hasWorkforceChanged == true)
        {
            //UpdateSettlementWorkforce(&settlement, workInstructions);
        }
    }
    else
    {
        if(settlement.needsToReorganizeWork == false)
            return;

        settlement.needsToReorganizeWork = false;

        if(settlement.GetPopulation() == 0)
        {
            for(auto &tile : settlement.GetTiles())
            {
                if(tile.Tile == settlement.GetLocation())
                    continue;

                tile.IsWorked = false;
            }

            return;
        }

        if(settlement.GetPopulation() > settlement.tiles.GetSize())
            return;

        auto workedTileCount = settlement.GetWorkedTiles();
        if(workedTileCount == settlement.GetPopulation() + 1)
            return;

        if(workedTileCount == settlement.GetTiles().GetSize())
            return;

        for(auto &tile : settlement.GetTiles())
        {
            if(tile.Tile == settlement.GetLocation())
                continue;

            tile.IsWorked = false;
        }

        auto workerCount = settlement.GetPopulation();
        
        auto placeWorkersOnTilesOfType = [&] (world::WorldBiomes biomeType) -> bool
        {
            for(auto &tile : settlement.GetTiles())
            {
                if(tile.IsWorked == true)
                    continue;

                if(tile.Tile->Biome->Type != biomeType)
                    continue;

                tile.IsWorked = true;
                workerCount--;

                if(workerCount == 0)
                    return true;
            }

            return false;
        };

        auto hasUsedAllWorkers = placeWorkersOnTilesOfType(world::WorldBiomes::STEPPE);

        if(hasUsedAllWorkers == true)
            return;

        hasUsedAllWorkers = placeWorkersOnTilesOfType(world::WorldBiomes::WOODS);

        if(hasUsedAllWorkers == true)
            return;

        placeWorkersOnTilesOfType(world::WorldBiomes::DESERT);
    }
}

void MachineMind::UpdateWorkforceBasedOnFreeMarket(settlement::Settlement &settlement) const
{
    //Fire tile based workers
    static auto scores = container::Array <JobScore> (128);
    scores.Reset();

    for(auto &job : settlement.GetResourceHandler().GetJobs())
    {
        if(job.GetTimeSpent() < 120)
            continue;

        if(job.GetTile() == nullptr)
            continue;

        auto tile = job.GetTile();

        if(tile->Tile->GetResource(settlement::ResourceTypes::FOOD) > 0 && settlement.GetResource(settlement::ResourceTypes::FOOD)->ScarcityDegree == 3)
            continue;

        if(tile->Tile->GetResource(settlement::ResourceTypes::FOOD) > 1 && settlement.GetResource(settlement::ResourceTypes::FOOD)->ScarcityDegree != 0)
            continue;

        auto generalScore = 0;
        for(auto &resource : settlement::BASIC_RESOURCES)
        {
            generalScore += tile->Tile->GetResource(resource) * (settlement.GetResource(resource)->AbundanceDegree - settlement.GetResource(resource)->ScarcityDegree);
        }

        if(generalScore <= 0)
            continue;

        *scores.Add() = JobScore{&job, generalScore};
    }

    scores.Sort([] (JobScore *first, JobScore *second) -> bool {return first->Score > second->Score;});

    for(auto &score : scores)
    {
        if(utility::RollD100Dice() > 5)
            break;

        settlement.FireWorker(score.Job);
    }

    //Fire non-tile based workers
    scores.Reset();

    for(auto &job : settlement.GetResourceHandler().GetJobs())
    {
        if(job.GetTimeSpent() < 120)
            continue;

        if(job.GetTile() != nullptr)
            continue;

        auto resource = settlement.GetResource(job.GetResource());

        auto generalScore = 0;
        
        generalScore += resource->AbundanceDegree;

        for(auto &input : resource->Type->InputResources)
        {
            auto inputResource = settlement.GetResource(input.Resource);
            generalScore += inputResource->ScarcityDegree * 2;
        }

        *scores.Add() = JobScore{&job, generalScore};
    }

    scores.Sort([] (JobScore *first, JobScore *second) -> bool {return first->Score > second->Score;});

    for(auto &score : scores)
    {
        if(utility::RollD100Dice() > 5)
            break;

        settlement.FireWorker(score.Job);
    }
}

void MachineMind::UpdateIntermediateResourceWorkforce(settlement::Settlement &settlement) const
{
    auto workerCount = settlement.GetFreeWorkerCount();
    if(workerCount == 0)
        return;

    static auto scores = container::Array <ResourceScore> (128);
    scores.Reset();

    for(auto &resourceType : settlement::INTERMEDIATE_RESOURCES)
    {
        auto resource = settlement.GetResource(resourceType);
        if(resource->AbundanceDegree != 0)
            continue;

        bool isInputScarce = false;
        for(auto &input : resource->Type->InputResources)
        {
            auto inputResource = settlement.GetResource(input.Resource);
            if(inputResource->ScarcityDegree == 3)
            {
                isInputScarce = true;
                break;
            }
        }

        if(isInputScarce == true)
            continue;

        *scores.Add() = {resource, resource->ScarcityDegree};
    }

    scores.Sort([] (ResourceScore *first, ResourceScore *second) -> bool {return first->Score > second->Score;});

    for(auto &score : scores)
    {
        if(utility::RollD100Dice() > 3)
            break;

        settlement::Cohort *chosenPop = nullptr;
        for(auto &pop : settlement.GetPopCohorts())
        {
            if(pop.Job != nullptr)
                continue;

            chosenPop = &pop;
            break;
        }

        settlement.HireWorker(score.Resource->Type->Type, chosenPop);

        workerCount--;

        if(workerCount == 0)
            break;
    }
}

void MachineMind::UpdateWorkforceBasedOnCentralPlan(settlement::Settlement &settlement) const
{
    if(settlement.GetPopulation() == 0)
    {
        return;
    }

    auto workerCount = settlement.GetPopulation();

    static auto scores = container::Array <TileScore> (128);
    scores.Reset();

    for(auto &tile : settlement.GetTiles())
    {
        if(tile.Tile == settlement.GetLocation())
            continue;

        if(tile.IsWorked == true)
            continue;

        auto foodScore = tile.Tile->GetResource(settlement::ResourceTypes::FOOD);

        auto generalScore = 0;
        for(auto &resource : settlement::BASIC_RESOURCES)
        {
            generalScore += tile.Tile->GetResource(resource) * settlement.GetResource(resource)->ScarcityDegree * (resource == settlement::ResourceTypes::FOOD ? 2 : 1);
        }

        *scores.Add() = TileScore{&tile, generalScore};
    }

    scores.Sort([] (TileScore *first, TileScore *second) -> bool {return first->GeneralScore > second->GeneralScore;});

    for(auto &score : scores)
    {
        if(utility::RollD100Dice() > 3)
            break;

        settlement::Cohort *chosenPop = nullptr;
        for(auto &pop : settlement.GetPopCohorts())
        {
            if(pop.Job != nullptr)
                continue;

            chosenPop = &pop;
            break;
        }

        settlement.HireWorker(score.Tile, chosenPop);

        workerCount--;

        if(workerCount == 0)
            break;
    }

    if(workerCount == 0)
        return;
}

void MachineMind::DecideBorders(Polity &polity) const
{
    for(auto &settlement : polity.GetSettlements())
    {
        if(settlement->CanGrowBorders() == false)
            continue;

        GrowBorders(settlement);

        settlement->needsToReorganizeWork = true;
    }
}

void MachineMind::GrowBorders(settlement::Settlement *settlement)
{
    if(settlement->tiles.GetSize() == settlement->tiles.GetCapacity())
        return;

    if(settlement->areNearbyTilesTaken == true)
        return;

    //static std::mutex mutex;

    //mutex.lock();

    auto findNewTileInRing = [&] (int distance) -> tile::WorldTile *
    {
        auto tileRing = settlement->location->GetTileRing(distance);

        auto findNewTile = [this, &tileRing] (world::WorldBiomes biomeType) -> tile::WorldTile *
        {
            for(auto &tile : tileRing)
            {
                if(tile->IsOwned())
                    continue;

                if(tile->Biome->Type != biomeType)
                    continue;

                return tile;
            }    
            return nullptr;
        };

        auto newTile = findNewTile(world::WorldBiomes::STEPPE);
        if(newTile == nullptr)
        {
            newTile = findNewTile(world::WorldBiomes::WOODS);
            if(newTile == nullptr)
            {
                newTile = findNewTile(world::WorldBiomes::DESERT);
                if(newTile == nullptr)
                {
                    newTile = findNewTile(world::WorldBiomes::MARINE);
                }
            }
        }

        return newTile;
    };

    auto newTile = findNewTileInRing(2);
    if(newTile != nullptr)
    {
        auto tile = settlement->tiles.Add();
        tile->Tile = newTile;
        tile->IsWorked = false;
        tile->ResetImprovement();

        newTile->AssertOwnership(settlement);
    }
    else
    {
        newTile = findNewTileInRing(3);
        if(newTile != nullptr)
        {
            auto tile = settlement->tiles.Add();
            tile->Tile = newTile;
            tile->IsWorked = false;
            tile->ResetImprovement();

            newTile->AssertOwnership(settlement);
        }
        else
        {
            settlement->areNearbyTilesTaken = true;
        }
    }

    settlement->cultureGrowth = 0;

    //mutex.unlock();
}