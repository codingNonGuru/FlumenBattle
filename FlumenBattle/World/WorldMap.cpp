#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenEngine/Thread/ThreadManager.h"

#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldAllocator.h"

using namespace world;

#define TILES_PER_BUFFER 1024

#define TILE_BUFFERS_PER_THREAD 8

TileBuffer::TileBuffer() : Tiles(TILES_PER_BUFFER) {}

void TileBuffer::Initialize(TileBufferBatch *batch) {Batch = batch;}

TileBuffer::~TileBuffer()
{
    Batch->Buffers.Remove(Tiles.GetStart());
}

TileBufferBatch::TileBufferBatch() : Buffers(TILE_BUFFERS_PER_THREAD)
{
    for(int i = 0; i < Buffers.GetCapacity(); ++i)
    {
        auto buffer = Buffers.Add();

        buffer->Initialize(this);
    }

    Buffers.Reset();
}

TileBufferBatch bufferBatches[engine::ThreadManager::GetThreadCount() + 2];

static auto bufferBatchMap = container::StaticMap <TileBufferBatch *, std::thread::id> (engine::ThreadManager::GetThreadCount() + 2);

auto lastBatchIndex = 0;

TileBuffer *GetUsableBuffer()
{
    const auto threadId = std::this_thread::get_id();

    auto batchPointer = bufferBatchMap.Get(threadId);
    if(batchPointer != nullptr)
    {
        auto batch = *batchPointer;

        auto buffer = batch->Buffers.Add();
        buffer->Tiles.Reset();

        return buffer;
    }

    batchPointer = bufferBatchMap.Add(threadId);

    *batchPointer = &bufferBatches[lastBatchIndex];
    lastBatchIndex++;

    auto batch = *batchPointer;
        
    auto buffer = batch->Buffers.Add();
    buffer->Tiles.Reset();

    return buffer;
}

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
}

const TileBuffer WorldMap::GetTileRing(WorldTile* tile, Integer range)
{
    auto buffer = GetUsableBuffer();

    for(Integer x = -range; x <= range; ++x)
    {
        for(Integer y = -range; y <= range; ++y)
        {
            for(Integer z = -range; z <= range; ++z)
            {
                if(x + y + z == 0 && abs(x) + abs(y) + abs(z) == range * 2)
                {
                    auto nearbyTile = GetTile(tile->HexCoordinates + Integer3(x, y, z));
                    if(nearbyTile != nullptr)
                    {
                        *buffer->Tiles.Add() = nearbyTile;
                    }
                }
            }
        }
    }

    return std::move(*buffer);
}

const TileBuffer WorldMap::GetNearbyTiles(WorldTile* tile, Integer range)
{
    auto buffer = GetUsableBuffer();

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
                        *buffer->Tiles.Add() = nearbyTile;
                    }
                }
            }
        }
    }

    return std::move(*buffer);
}

const container::Block <WorldTile *, 6> WorldMap::GetNearbyTiles(WorldTile* tile)
{
    auto nearbyTiles = container::Block <WorldTile *, 6>();

    int i = 0;
    for(Integer x = -1; x <= 1; ++x)
    {
        for(Integer y = -1; y <= 1; ++y)
        {
            for(Integer z = -1; z <= 1; ++z)
            {
                if(x + y + z == 0)
                {
                    auto nearbyTile = GetTile(tile->HexCoordinates + Integer3(x, y, z));
                    if(nearbyTile != nullptr && nearbyTile != tile)
                    {
                        *nearbyTiles[i] = nearbyTile;
                        i++;
                    }
                }
            }
        }
    }

    return nearbyTiles;
}

WorldTile* WorldMap::GetTile(Integer3 position) const
{
    return tiles.Get(position.x + position.z / 2, position.z);
}

WorldTile* WorldMap::GetTile(Integer2 position) const
{
    return tiles.Get(position.x, position.y);
}

WorldTile* WorldMap::GetTile(Float2 position) const
{
    position /= TILE_DISTANCING;
    position.y /= 0.8666f;
        
    if((int)position.y % 2 == 1)
    {
        position.x -= 0.5f;
    }

    return tiles.Get((int)position.x, (int)position.y);
}

WorldTile* WorldMap::GetRandomTile()
{
    auto i = utility::GetRandom(0, tiles.GetWidth() - 1);
    auto j = utility::GetRandom(0, tiles.GetHeight() - 1);

    return tiles.Get(i, j);
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
        if(tile->group == nullptr && tile->Type == WorldTiles::LAND)
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

WorldTile* WorldMap::GetCenterTile()
{
    return tiles.Get(tiles.GetWidth() / 2, tiles.GetHeight() / 2);
}