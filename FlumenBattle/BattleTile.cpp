#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/BattleMap.h"

const Array <BattleTile*> & BattleTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

BattleTile* BattleTile::GetEmptyTileInRange(Integer range)
{
    return Map->GetEmptyTileAroundTile(this, range);
}

BattleTile * BattleTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = HexCoordinates + direction;
    return Map->GetTile(coordinates);
}