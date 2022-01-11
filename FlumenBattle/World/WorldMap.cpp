#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"

using namespace world;

static Array <WorldTile*> nearbyTiles;

WorldMap::WorldMap(Length size) 
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

    for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile)
    {
        tile->Initialize();
    }
}

const Array<WorldTile*> & WorldMap::GetNearbyTiles(WorldTile* tile, Integer range)
{
    nearbyTiles.Reset();

    for(Integer x = -range; x <= range; ++x)
    {
        for(Integer y = -range; y <= range; ++y)
        {
            for(Integer z = -range; z <= range; ++z)
            {
                if(x + y + z == 0)
                {
                    auto nearbyTile = GetTile(tile->HexCoordinates + Integer3(x, y, z));
                    if(nearbyTile != nullptr)
                    {
                        *nearbyTiles.Allocate() = nearbyTile;
                    }
                }
            }
        }
    }

    return nearbyTiles;
}

WorldTile* WorldMap::GetTile(Integer3 position)
{
    return tiles.Get(position.x + position.z / 2, position.z);
}

WorldTile* WorldMap::GetRandomTile()
{
    auto i = utility::GetRandom(0, tiles.GetWidth() - 1);
    auto j = utility::GetRandom(0, tiles.GetHeight() - 1);

    return tiles.Get(i, j);
}

WorldTile* WorldMap::GetEmptyRandomTile()
{
    /*while(true)
    {
        auto tile = GetRandomTile();
        if(tile->Combatant == nullptr)
        {
            return tile;
        }
    }*/
    return nullptr;
}

WorldTile* WorldMap::GetEmptyTileAroundTile(WorldTile * tile, Integer range)
{
    auto& nearbyTiles = tile->GetNearbyTiles(range);
    while(true)
    {
        auto index = utility::GetRandom(0, nearbyTiles.GetSize() - 1);
        auto otherTile = *nearbyTiles.Get(index);
        //if(otherTile->Combatant == nullptr)
        //{
            return otherTile;
        //}
    }
}

WorldTile* WorldMap::GetCenterTile()
{
    return tiles.Get(tiles.GetWidth() / 2, tiles.GetHeight() / 2);
}