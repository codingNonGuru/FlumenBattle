#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/GroupAllocator.h"

using namespace world;

WorldAllocator::WorldAllocator()
{
    nearbyTileMemory = container::Array <WorldTilePointer>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);

    worldTileMemory = container::Grid <WorldTile>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);

    auto polityCount = WorldGenerator::Get()->GetMaximumPolityCount(MAXIMUM_WORLD_SIZE);
    polityMemory = container::Pool<Polity>::PreallocateMemory(polityCount);

    settlement::SettlementAllocator::Get()->PreallocateMaximumMemory();

    world::GroupAllocator::Get()->PreallocateMaximumMemory();
}

void WorldAllocator::AllocateMap(WorldMap &map, container::Array <WorldTilePointer> &nearbyTiles, int size)
{
    nearbyTiles.Initialize(size * size, nearbyTileMemory);

    auto height = size;
    map.tiles.Initialize(size, height, worldTileMemory);
}

void WorldAllocator::AllocateSociety(int worldSize)
{
    auto scene = WorldScene::Get();

    auto polityCount = WorldGenerator::Get()->GetMaximumPolityCount(worldSize);
    scene->polities.Initialize(polityCount, polityMemory);

    settlement::SettlementAllocator::Get()->AllocateWorldMemory(worldSize);

    world::GroupAllocator::Get()->AllocateWorldMemory(worldSize);
}