#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"

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