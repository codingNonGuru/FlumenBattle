#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Science/Technology.h"

using namespace world::polity;

void MachineMind::MakeDecision(Polity &polity) const
{
    UpdateWorkforce(polity);

    for(auto &settlement : polity.GetSettlements())
    {
        if(settlement->GetCurrentProduction()->Is(settlement::ProductionOptions::NONE) == false)
            continue;

        settlement->DecideProduction();
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
        if(settlement->needsToReorganizeWork == false)
            continue;

        settlement->needsToReorganizeWork = false;

        if(settlement->GetPopulation() == 0)
        {
            for(auto &tile : settlement->tiles)
            {
                if(tile.Tile == settlement->location)
                    continue;

                tile.IsWorked = false;
            }
            continue;
        }

        if(settlement->GetPopulation() > settlement->tiles.GetSize())
            continue;

        auto workedTileCount = settlement->GetWorkedTiles();
        if(workedTileCount == settlement->GetPopulation() + 1)
            continue;

        if(workedTileCount == settlement->tiles.GetSize())
            continue;

        for(auto &tile : settlement->tiles)
        {
            if(tile.Tile == settlement->location)
                continue;

            tile.IsWorked = false;
        }

        auto workerCount = settlement->GetPopulation();
        
        auto placeWorkersOnTilesOfType = [&] (world::WorldBiomes biomeType) -> bool
        {
            for(auto &tile : settlement->tiles)
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
            continue;

        hasUsedAllWorkers = placeWorkersOnTilesOfType(world::WorldBiomes::WOODS);

        if(hasUsedAllWorkers == true)
            continue;

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