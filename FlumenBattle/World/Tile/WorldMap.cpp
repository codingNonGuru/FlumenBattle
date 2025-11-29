#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenEngine/Thread/ThreadManager.h"
#include "FlumenEngine/Thread/HexBufferManager.h"

#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/Tile/River.h"

using namespace world::tile;

WorldMap::WorldMap() {}

WorldMap::WorldMap(Length size) 
{
    WorldAllocator::Get()->AllocateMap(*this, size);

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
            tile->map = this;
        }
    }

    for(auto &edge : GetEdges())
    {
        edge.River = nullptr;

        edge.Downstream = nullptr;
    }

    rivers.Initialize(128);
}

WorldTile* WorldMap::GetTileFromPosition(Float2 position) const
{
    position /= TILE_DISTANCING;
    position.y /= 0.8666f;
        
    if((int)position.y % 2 == 1)
    {
        position.x -= 0.5f;
    }

    return tiles.Get((int)position.x, (int)position.y);
}

WorldTile* WorldMap::GetRandomLandTile()
{
    while(true)
    {
        auto tile = GetRandomTile();
        if(tile->Type == WorldTiles::LAND)
        {
            return tile;
        }
    }
    return nullptr;
}

WorldTile* WorldMap::GetEmptyRandomTile(bool isLand)
{
    while(true)
    {
        auto tile = GetRandomTile();
        if(tile->Type == WorldTiles::LAND)
        {
            return tile;
        }
    }
    return nullptr;
}

WorldTile* WorldMap::GetEmptyTileAroundTile(WorldTile * tile, Integer range)
{
    auto nearbyTiles = tile->GetNearbyTiles(range);
    while(true)
    {
        auto index = utility::GetRandom(0, nearbyTiles.GetSize() - 1);
        auto otherTile = *nearbyTiles.Tiles.Get(index);
        //if(otherTile->Combatant == nullptr)
        //{
            return otherTile;
        //}
    }
}

River *WorldMap::AddRiver(WorldEdge *firstSegment)
{
    auto newRiver = rivers.Add();
    newRiver->Initialize();

    newRiver->AddSegment(firstSegment);
}