#include "SettlementTile.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"

using namespace world::settlement;

int SettlementTile::GetImprovementBonus(ResourceTypes resource) const
{
    if(ImprovementType == nullptr)
        return 0;
    else 
        return ImprovementType->GetBonus(resource, Tile);
}

void SettlementTile::AddImprovement(TileImprovements type)
{
    ImprovementType = TileImprovementFactory::Get()->BuildImprovementType(type);
}

bool SettlementTile::HasImprovement(TileImprovements type) const 
{
    if(ImprovementType == nullptr)
        return false;

    return ImprovementType->Type == type;
}