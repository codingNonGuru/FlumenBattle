#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"

#define MAXIMUM_TILES_PER_SETTLEMENT 19

#define MAXIMUM_CONDITIONS_PER_SETTLEMENT 32

#define MAXIMUM_BUILDINGS_PER_SETTLEMENT 16

#define MAXIMUM_AFFLICTIONS_PER_SETTLEMENT 16

#define MAXIMUM_EVENTS_PER_SETTLEMENT 32

#define MAXIMUM_PATHS_PER_SETTLEMENT 8

#define AVERAGE_SEGMENTS_PER_PATH 4

#define MODIFIERS_PER_SETTLEMENT 32

using namespace world::settlement;

void SettlementAllocator::PreallocateMaximumMemory()
{
    auto worldGenerator = WorldGenerator::Get();

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(MAXIMUM_WORLD_SIZE);

    settlementMemory = container::Pool <Settlement>::PreallocateMemory(settlementCount);

    groupDynamicsMemory = container::Pool <group::GroupDynamics>::PreallocateMemory(settlementCount);

    tileMemory = container::PoolAllocator <SettlementTile>::PreallocateMemory(settlementCount, MAXIMUM_TILES_PER_SETTLEMENT);

    afflictionMemory = container::PoolAllocator <Affliction>::PreallocateMemory (settlementCount, MAXIMUM_AFFLICTIONS_PER_SETTLEMENT);

    eventMemory = container::PoolAllocator <Event>::PreallocateMemory (settlementCount, MAXIMUM_EVENTS_PER_SETTLEMENT);

    productionMemory = container::Pool <SettlementProduction>::PreallocateMemory (settlementCount);

    conditionManagerMemory = container::Pool <ConditionManager>::PreallocateMemory (settlementCount);

    conditionMemory = container::PoolAllocator <Condition>::PreallocateMemory (settlementCount, MAXIMUM_CONDITIONS_PER_SETTLEMENT);

    buildingManagerMemory = container::Pool <BuildingManager>::PreallocateMemory (settlementCount);

    buildingMemory = container::PoolAllocator <Building>::PreallocateMemory (settlementCount, MAXIMUM_BUILDINGS_PER_SETTLEMENT);

    pathMemory = container::Pool <Path>::PreallocateMemory (settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT);

    pathSegmentMemory = container::Pool <PathSegment>::PreallocateMemory (settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT * AVERAGE_SEGMENTS_PER_PATH);

    linkMemory = container::PoolAllocator <Link>::PreallocateMemory (settlementCount, MAXIMUM_PATHS_PER_SETTLEMENT);

    modifierMemory = container::ArrayAllocator <Modifier>::PreallocateMemory (settlementCount, MODIFIERS_PER_SETTLEMENT);
}

void SettlementAllocator::AllocateWorldMemory(int worldSize)
{
    auto worldGenerator = WorldGenerator::Get();

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(worldSize);

    settlements.Initialize(settlementCount, settlementMemory);

    groupDynamics.Initialize(settlementCount, groupDynamicsMemory);

    paths.Initialize(settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT, pathMemory);

    pathSegments.Initialize(settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT * AVERAGE_SEGMENTS_PER_PATH, pathSegmentMemory);

    tileAllocator = container::PoolAllocator <SettlementTile> (settlementCount, MAXIMUM_TILES_PER_SETTLEMENT, tileMemory);

    afflictionAllocator = container::PoolAllocator <Affliction> (settlementCount, MAXIMUM_AFFLICTIONS_PER_SETTLEMENT, afflictionMemory);

    eventAllocator = container::PoolAllocator <Event> (settlementCount, MAXIMUM_EVENTS_PER_SETTLEMENT, eventMemory);

    productionAllocator = container::Pool <SettlementProduction> (settlementCount, productionMemory);

    conditionManagerAllocator = container::Pool <ConditionManager> (settlementCount, conditionManagerMemory);

    conditionAllocator = container::PoolAllocator <Condition> (settlementCount, MAXIMUM_CONDITIONS_PER_SETTLEMENT, conditionMemory);

    buildingManagerAllocator = container::Pool <BuildingManager> (settlementCount, buildingManagerMemory);

    buildingAllocator = container::PoolAllocator <Building> (settlementCount, MAXIMUM_BUILDINGS_PER_SETTLEMENT, buildingMemory);

    linkAllocator = container::PoolAllocator <Link> (settlementCount, MAXIMUM_PATHS_PER_SETTLEMENT, linkMemory);

    modifierAllocator = container::ArrayAllocator <Modifier> (settlementCount, MODIFIERS_PER_SETTLEMENT, modifierMemory);
}

Settlement * SettlementAllocator::Allocate()
{
    auto settlement = settlements.Add();

    settlement->groupDynamics = groupDynamics.Add();

    auto &tiles = settlement->GetTiles();
    tiles.Initialize(tileAllocator);

    settlement->afflictions.Initialize(afflictionAllocator);

    settlement->events.Initialize(eventAllocator);

    settlement->currentProduction = productionAllocator.Add();

    settlement->conditionManager = conditionManagerAllocator.Add();

    ConditionAllocator::Allocate(conditionAllocator, *settlement->conditionManager);

    ModifierAllocator::Allocate(modifierAllocator, settlement->modifierManager);

    settlement->buildingManager = buildingManagerAllocator.Add();

    BuildingSetAllocator::Allocate(buildingAllocator, *settlement->buildingManager);

    settlement->links.Initialize(linkAllocator);

    return settlement;
}

Path * SettlementAllocator::AllocatePath(Settlement *firstSettlement, Settlement *secondSettlement)
{
    auto path = paths.Add();

    //firstSettlement->links.Initialize(linkAllocator);

    //secondSettlement->links.Initialize(linkAllocator);

    return path;
}

PathSegment * SettlementAllocator::AllocateSegment()
{
    auto segment = pathSegments.Add();

    return segment;
}