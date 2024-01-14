#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/PolityAllocator.h"

using namespace world;

WorldAllocator::WorldAllocator()
{
    for(int i = 0; i < 4; ++i)
    {
        nearbyTileMemories[i] = container::Array <WorldTilePointer>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);
    }

    worldTileMemory = container::Grid <WorldTile>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);

    polity::PolityAllocator::Get()->PreallocateMaximumMemory();

    settlement::SettlementAllocator::Get()->PreallocateMaximumMemory();

    world::group::GroupAllocator::Get()->PreallocateMaximumMemory();

    world::character::CharacterAllocator::Get()->PreallocateMaximumMemory();
}

void WorldAllocator::AllocateMap(WorldMap &map, container::SmartBlock< container::Array <WorldTilePointer>, 4> &nearbyTileBuffers, int size)
{
    for(int i = 0; i < 4; ++i)
    {
        nearbyTileBuffers.Get(i)->Initialize(size * size, nearbyTileMemories[i]);
    }

    auto height = size;
    map.tiles.Initialize(size, height, worldTileMemory);
}

void WorldAllocator::AllocateSociety(int worldSize)
{
    polity::PolityAllocator::Get()->AllocateWorldMemory(worldSize);

    settlement::SettlementAllocator::Get()->AllocateWorldMemory(worldSize);

    world::group::GroupAllocator::Get()->AllocateWorldMemory(worldSize);

    world::character::CharacterAllocator::Get()->AllocateWorldMemory(worldSize);
}