#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/BattleMap.h"

using namespace battle;

static Array <BattleTile*> nearbyTiles;

BattleMap::BattleMap(Length size) 
{
    nearbyTiles.Initialize(size * size);

    auto height = size;
    tiles.Initialize(size, height);

    tiles.Bound();

    for(Integer i = 0; i < tiles.GetWidth(); ++i)
    {
        for(Integer j = 0; j < tiles.GetHeight(); ++j)
        {
            auto x = float(i);
            auto y = float(j) * 0.866f;

            if(j % 2 == 1)
            {
                x += 0.5f;
            }

            auto tile = tiles.Get(i, j);
            *tile = {Position2(x, y) * TILE_DISTANCING, Integer2(i, j)};
            tile->Map = this;
        }
    }
}

BattleTile* BattleMap::GetEmptyRandomTile()
{
    while(true)
    {
        auto tile = GetRandomTile();
        if(tile->Combatant == nullptr)
        {
            return tile;
        }
    }
}

BattleTile* BattleMap::GetEmptyTileAroundTile(BattleTile * tile, Integer range)
{
    auto nearbyTiles = tile->GetNearbyTiles(range);
    while(true)
    {
        auto index = utility::GetRandom(0, nearbyTiles.GetSize() - 1);
        auto otherTile = *nearbyTiles.Tiles.Get(index);
        if(otherTile->Combatant == nullptr && otherTile->IsObstacle == false)
        {
            return otherTile;
        }
    }
}