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
#include "FlumenBattle/World/SimulationMap.h"
#include "FlumenBattle/World/SimulationDomain.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/Config.h"

using namespace world;

WorldAllocator::WorldAllocator()
{
    std::cout<<"Memory size of a World Tile is "<<sizeof(WorldTile)<<"\n";

    for(int i = 0; i < 4; ++i)
    {
        nearbyTileMemories[i] = container::Array <WorldTilePointer>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);
    }

    worldTileMemory = container::Grid <WorldTile>::PreallocateMemory(MAXIMUM_WORLD_SIZE * MAXIMUM_WORLD_SIZE);

    auto size = MAXIMUM_WORLD_SIZE / TILES_PER_SIMULATION_DOMAIN;
    simulationMemory = container::Grid <SimulationDomain>::PreallocateMemory(size * size);

    static const auto GROUP_BATCH_TILE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_BATCH_TILE_SIZE).Integer;

    size = MAXIMUM_WORLD_SIZE / GROUP_BATCH_TILE_SIZE;
    batchMemory = container::Grid <group::GroupBatch>::PreallocateMemory(size * size);

    static const auto GROUPS_PER_BATCH = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUPS_PER_BATCH).Integer;

    groupBatchMemory = container::PoolAllocator <group::Group *>::PreallocateMemory(size * size, GROUPS_PER_BATCH);

    polity::PolityAllocator::Get()->PreallocateMaximumMemory();

    settlement::SettlementAllocator::Get()->PreallocateMaximumMemory();

    world::group::GroupAllocator::Get()->PreallocateMaximumMemory();

    world::character::CharacterAllocator::Get()->PreallocateMaximumMemory();
}

void WorldAllocator::AllocateMap(WorldMap &map/*, container::SmartBlock< container::Array <WorldTilePointer>, 4> &nearbyTileBuffers*/, int size)
{
    /*for(int i = 0; i < 4; ++i)
    {
        nearbyTileBuffers.Get(i)->Initialize(size * size, nearbyTileMemories[i]);
    }*/

    auto height = size;
    map.tiles.Initialize(size, height, worldTileMemory);

    auto simulationSize = size / TILES_PER_SIMULATION_DOMAIN;
    SimulationMap::Get()->domains.Initialize(simulationSize, simulationSize, simulationMemory);

    static const auto GROUP_BATCH_TILE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_BATCH_TILE_SIZE).Integer;

    auto batchCount = size / GROUP_BATCH_TILE_SIZE;
    auto &batches = group::GroupBatchMap::Get()->batches;
    batches.Initialize(batchCount, batchCount, batchMemory);

    static const auto GROUPS_PER_BATCH = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUPS_PER_BATCH).Integer;

    groupBatchAllocator = container::PoolAllocator <group::Group *> (batchCount * batchCount, GROUPS_PER_BATCH, groupBatchMemory);
    for(auto batch = batches.GetStart(); batch != batches.GetEnd(); ++batch)
    {
        group::GroupBatchAllocator::AllocateBatch(*batch, groupBatchAllocator);
    }
}

void WorldAllocator::AllocateSociety(int worldSize)
{
    polity::PolityAllocator::Get()->AllocateWorldMemory(worldSize);

    settlement::SettlementAllocator::Get()->AllocateWorldMemory(worldSize);

    world::group::GroupAllocator::Get()->AllocateWorldMemory(worldSize);

    world::character::CharacterAllocator::Get()->AllocateWorldMemory(worldSize);
}