#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
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

void MachineMind::UpdateWorkforceBasedOnFreeMarket(settlement::Settlement &settlement) const
{
    for(auto &job : settlement.GetResourceHandler().GetJobs())
    {
        if(job.GetTile() != nullptr)
            continue;

        auto resource = settlement.GetResource(job.GetResource());
        if(resource->AbundanceDegree == 3)
        {
            settlement.FireWorker(&job);
            continue;
        }

        for(auto &input : resource->Type->InputResources)
        {
            auto inputResource = settlement.GetResource(input.Resource);
            if(inputResource->ScarcityDegree == 3)
            {
                settlement.FireWorker(&job);
                break;
            }
        }
    }

    for(auto &cohort : settlement.GetPopCohorts())
    {
        if(cohort.Job != nullptr)
            continue;        

        auto resourceType = *settlement::INTERMEDIATE_RESOURCES.GetRandom();
        
        auto resource = settlement.GetResource(resourceType);
        if(resource->ScarcityDegree != 3)
            continue;
        
        bool isInputScarce = false;
        for(auto &input : resource->Type->InputResources)
        {
            auto inputResource = settlement.GetResource(input.Resource);
            if(inputResource->ScarcityDegree != 0)
            {
                isInputScarce = true;
                break;
            }
        }

        if(isInputScarce == false)
        {
            settlement.HireWorker(resourceType, &cohort);
            continue;
        }
    }
}

void MachineMind::UpdateWorkforceBasedOnCentralPlan(settlement::Settlement &settlement) const
{
    struct TileScore
    {
        settlement::SettlementTile *Tile;

        int FoodScore;

        int GeneralScore;
    };

    if(settlement.needsToReorganizeWork == false)
        return;

    settlement.needsToReorganizeWork = false;

    settlement.FireAllWorkers();

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

        auto foodScore = tile.Tile->GetResource(settlement::ResourceTypes::FOOD);

        auto generalScore = 0;
        for(auto &resource : settlement::BASIC_RESOURCES)
        {
            generalScore += tile.Tile->GetResource(resource);
        }

        *scores.Add() = TileScore{&tile, foodScore, generalScore};
    }

    scores.Sort([] (TileScore *first, TileScore *second) -> bool {return first->GeneralScore > second->GeneralScore;});

    scores.Sort([] (TileScore *first, TileScore *second) -> bool {return first->FoodScore > second->FoodScore;});

    auto neededFood = settlement.GetPopulationHandler().GetPotentialMidtermConsumption(settlement::ResourceTypes::FOOD);

    for(auto &score : scores)
    {
        settlement.HireWorker(score.Tile);

        workerCount--;
        if(workerCount == 0)
            break;

        auto foodOutput = settlement.GetResourceHandler().GetPotentialMidtermOutput(settlement::ResourceTypes::FOOD);
        if(foodOutput > neededFood)
            break;
    }

    if(workerCount == 0)
        return;

    for(auto &score : scores)
    {
        if(score.Tile->IsWorked == true)
            continue;

        if(score.GeneralScore < 4)
            continue;

        settlement.HireWorker(score.Tile);
        workerCount--;

        if(workerCount == 0)
            break;
    }

    if(workerCount == 0)
        return;
}

void MachineMind::UpdateWorkforce(settlement::Settlement &settlement) const
{
    if(settlement.IsDeepSettlement() == true)
    {
        UpdateWorkforceBasedOnCentralPlan(settlement);

        UpdateWorkforceBasedOnFreeMarket(settlement);
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