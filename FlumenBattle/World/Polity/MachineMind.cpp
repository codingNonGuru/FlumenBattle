#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
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

        if(settlement->population == 0)
        {
            for(auto &tile : settlement->tiles)
            {
                if(tile.Tile == settlement->location)
                    continue;

                tile.IsWorked = false;
            }
            continue;
        }

        if(settlement->population > settlement->tiles.GetSize())
            continue;

        auto workedTileCount = settlement->GetWorkedTiles();
        if(workedTileCount == settlement->population + 1)
            continue;

        if(workedTileCount == settlement->tiles.GetSize())
            continue;

        for(auto &tile : settlement->tiles)
        {
            if(tile.Tile == settlement->location)
                continue;

            tile.IsWorked = false;
        }

        auto workerCount = settlement->population;
        
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