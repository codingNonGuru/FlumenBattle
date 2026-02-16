#include "TileImprovement.h"
#include "FlumenBattle/World/Tile/WorldTile.h"

using namespace world::settlement;

int TileImprovementType::GetBonus(ResourceTypes resource, tile::WorldTile *tile) const
{
    if(Resource != resource)
        return 0;

    auto baseYield = tile->GetResource(resource);
    if(baseYield >= 3)
        return 2;
    else if(baseYield >= 1)
        return 1;
    else
        return 0;
}

const TileImprovementType *TileImprovementFactory::BuildImprovementType(TileImprovements improvementType)
{
    switch(improvementType)
    {
    case TileImprovements::FARM:
        {
            static const TileImprovementType improvement = {TileImprovements::FARM, ResourceTypes::FOOD, "Farm", "FarmImprovement", 100};
            return &improvement;
        }
    case TileImprovements::PASTURE:
        {
            static const TileImprovementType improvement = {TileImprovements::PASTURE, ResourceTypes::FIBER, "Pasture", "PastureImprovement", 100};
            return &improvement;
        }
    case TileImprovements::CLAY_PIT:
        {
            static const TileImprovementType improvement = {TileImprovements::CLAY_PIT, ResourceTypes::CLAY, "Clay pit", "ClayPitImprovement", 100};
            return &improvement;
        }
    case TileImprovements::SAW_MILL:
        {
            static const TileImprovementType improvement = {TileImprovements::SAW_MILL, ResourceTypes::TIMBER, "Saw mill", "SawMillImprovement", 100};
            return &improvement;
        }
    case TileImprovements::MINE:
        {
            static const TileImprovementType improvement = {TileImprovements::MINE, ResourceTypes::ORE, "Mine", "MineImprovement", 100};
            return &improvement;
        }
    }
}