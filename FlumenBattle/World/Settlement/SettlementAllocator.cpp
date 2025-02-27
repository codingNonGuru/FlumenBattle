#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/Group/GroupEssence.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/Config.h"

#define MAXIMUM_TILES_PER_SETTLEMENT 37

#define MAXIMUM_CONDITIONS_PER_SETTLEMENT 32

#define MAXIMUM_BUILDINGS_PER_SETTLEMENT 16

#define MAXIMUM_AFFLICTIONS_PER_SETTLEMENT 16

#define MAXIMUM_EVENTS_PER_SETTLEMENT 32

#define AVERAGE_SEGMENTS_PER_PATH 4

#define MODIFIERS_PER_SETTLEMENT 32

#define ADVENTURERS_PER_SETTLEMENT 8

#define MERCHANTS_PER_SETTLEMENT 8

#define BANDITS_PER_SETTLEMENT 8

#define PATROLS_PER_SETTLEMENT 8

#define GARRISONS_PER_SETTLEMENT 4

#define RAIDERS_PER_SETTLEMENT 4

#define POPULATION_NEED_COUNT 6

using namespace world::settlement;

void SettlementAllocator::PreallocateMaximumMemory()
{
    static const auto MAXIMUM_WORLD_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WORLD_SIZE).Integer;

    static const auto MAXIMUM_PATHS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_PATHS_PER_SETTLEMENT).Integer;

    static const auto EXPLORATIONS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::EXPLORATIONS_PER_SETTLEMENT).Integer;

    std::cout<<"Memory size of a Settlement is "<<sizeof(Settlement)<<"\n";

    auto worldGenerator = WorldGenerator::Get();

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(MAXIMUM_WORLD_SIZE);

    settlementMemory = container::Pool <Settlement>::PreallocateMemory(settlementCount);

    groupDynamicsMemory = container::Pool <group::GroupDynamics>::PreallocateMemory(settlementCount);

    tileMemory = container::PoolAllocator <SettlementTile>::PreallocateMemory(settlementCount, MAXIMUM_TILES_PER_SETTLEMENT);

    afflictionMemory = container::PoolAllocator <Affliction>::PreallocateMemory (settlementCount, MAXIMUM_AFFLICTIONS_PER_SETTLEMENT);

    eventMemory = container::PoolAllocator <Event>::PreallocateMemory (settlementCount, MAXIMUM_EVENTS_PER_SETTLEMENT);

    productionMemory = container::Pool <SettlementProduction>::PreallocateMemory (settlementCount * 2);

    conditionManagerMemory = container::Pool <ConditionManager>::PreallocateMemory (settlementCount);

    conditionMemory = container::PoolAllocator <Condition>::PreallocateMemory (settlementCount, MAXIMUM_CONDITIONS_PER_SETTLEMENT);

    buildingManagerMemory = container::Pool <BuildingManager>::PreallocateMemory (settlementCount);

    buildingMemory = container::PoolAllocator <Building>::PreallocateMemory (settlementCount, MAXIMUM_BUILDINGS_PER_SETTLEMENT);

    pathMemory = container::Pool <Path>::PreallocateMemory (settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT);

    pathSegmentMemory = container::Pool <PathSegment>::PreallocateMemory (settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT * AVERAGE_SEGMENTS_PER_PATH);

    linkMemory = container::PoolAllocator <Link>::PreallocateMemory (settlementCount, MAXIMUM_PATHS_PER_SETTLEMENT);

    modifierMemory = container::ArrayAllocator <Modifier>::PreallocateMemory (settlementCount, MODIFIERS_PER_SETTLEMENT);

    resourceMemory = container::ArrayAllocator <Resource>::PreallocateMemory (settlementCount, GOODS_TYPES_COUNT);

    adventurerMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, ADVENTURERS_PER_SETTLEMENT);

    merchantMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, MERCHANTS_PER_SETTLEMENT);

    banditMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, BANDITS_PER_SETTLEMENT);

    patrolMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, PATROLS_PER_SETTLEMENT);

    garrisonMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, GARRISONS_PER_SETTLEMENT);

    raiderMemory = container::PoolAllocator <group::GroupEssence>::PreallocateMemory (settlementCount, RAIDERS_PER_SETTLEMENT);

    needMemory = container::ArrayAllocator <Need>::PreallocateMemory (settlementCount, POPULATION_NEED_COUNT);

    explorationMemory = container::PoolAllocator <ExploreResult>::PreallocateMemory (settlementCount, EXPLORATIONS_PER_SETTLEMENT);
}

void SettlementAllocator::AllocateWorldMemory(int worldSize)
{
    static const auto MAXIMUM_PATHS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_PATHS_PER_SETTLEMENT).Integer;

    static const auto EXPLORATIONS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::EXPLORATIONS_PER_SETTLEMENT).Integer;

    static const auto worldGenerator = WorldGenerator::Get();

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(worldSize);

    settlements.Initialize(settlementCount, settlementMemory);

    groupDynamics.Initialize(settlementCount, groupDynamicsMemory);

    paths.Initialize(settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT, pathMemory);

    pathSegments.Initialize(settlementCount * MAXIMUM_PATHS_PER_SETTLEMENT * AVERAGE_SEGMENTS_PER_PATH, pathSegmentMemory);

    tileAllocator = container::PoolAllocator <SettlementTile> (settlementCount, MAXIMUM_TILES_PER_SETTLEMENT, tileMemory);

    afflictionAllocator = container::PoolAllocator <Affliction> (settlementCount, MAXIMUM_AFFLICTIONS_PER_SETTLEMENT, afflictionMemory);

    eventAllocator = container::PoolAllocator <Event> (settlementCount, MAXIMUM_EVENTS_PER_SETTLEMENT, eventMemory);

    productionAllocator = container::Pool <SettlementProduction> (settlementCount * 2, productionMemory);

    conditionManagerAllocator = container::Pool <ConditionManager> (settlementCount, conditionManagerMemory);

    conditionAllocator = container::PoolAllocator <Condition> (settlementCount, MAXIMUM_CONDITIONS_PER_SETTLEMENT, conditionMemory);

    buildingManagerAllocator = container::Pool <BuildingManager> (settlementCount, buildingManagerMemory);

    buildingAllocator = container::PoolAllocator <Building> (settlementCount, MAXIMUM_BUILDINGS_PER_SETTLEMENT, buildingMemory);

    linkAllocator = container::PoolAllocator <Link> (settlementCount, MAXIMUM_PATHS_PER_SETTLEMENT, linkMemory);

    modifierAllocator = container::ArrayAllocator <Modifier> (settlementCount, MODIFIERS_PER_SETTLEMENT, modifierMemory);

    resourceAllocator = container::ArrayAllocator <Resource> (settlementCount, GOODS_TYPES_COUNT, resourceMemory);

    adventurerAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, ADVENTURERS_PER_SETTLEMENT, adventurerMemory);

    merchantAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, MERCHANTS_PER_SETTLEMENT, merchantMemory);

    banditAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, BANDITS_PER_SETTLEMENT, banditMemory);

    patrolAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, PATROLS_PER_SETTLEMENT, patrolMemory);

    garrisonAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, GARRISONS_PER_SETTLEMENT, garrisonMemory);

    raiderAllocator = container::PoolAllocator <group::GroupEssence> (settlementCount, RAIDERS_PER_SETTLEMENT, raiderMemory);

    needAllocator = container::ArrayAllocator <Need> (settlementCount, POPULATION_NEED_COUNT, needMemory);

    explorationAllocator = container::PoolAllocator <ExploreResult> (settlementCount, EXPLORATIONS_PER_SETTLEMENT, explorationMemory);
}

Settlement * SettlementAllocator::Allocate()
{
    auto settlement = settlements.Add();

    settlement->groupDynamics = groupDynamics.Add();

    settlement->groupDynamics->adventurers.Initialize(adventurerAllocator);

    settlement->groupDynamics->merchants.Initialize(merchantAllocator);

    settlement->groupDynamics->bandits.Initialize(banditAllocator);

    settlement->groupDynamics->patrols.Initialize(patrolAllocator);

    settlement->groupDynamics->garrisons.Initialize(garrisonAllocator);

    settlement->groupDynamics->raiders.Initialize(raiderAllocator);

    auto &tiles = settlement->GetTiles();
    tiles.Initialize(tileAllocator);

    settlement->afflictions.Initialize(afflictionAllocator);

    settlement->events.Initialize(eventAllocator);

    settlement->buildingProduction = productionAllocator.Add();

    settlement->groupProduction = productionAllocator.Add();

    settlement->conditionManager = conditionManagerAllocator.Add();

    ConditionAllocator::Allocate(conditionAllocator, *settlement->conditionManager);

    ModifierAllocator::Allocate(modifierAllocator, settlement->modifierManager);

    settlement->buildingManager = buildingManagerAllocator.Add();

    BuildingSetAllocator::Allocate(buildingAllocator, *settlement->buildingManager);

    settlement->links.Initialize(linkAllocator);

    ResourceAllocator::Allocate(resourceAllocator, settlement->resourceHandler);

    PopAllocator::Allocate(needAllocator, settlement->popHandler);

    settlement->finishedExplorations.Initialize(explorationAllocator);

    return settlement;
}

Path * SettlementAllocator::AllocatePath(Settlement *firstSettlement, Settlement *secondSettlement)
{
    auto path = paths.Add();

    return path;
}

PathSegment * SettlementAllocator::AllocateSegment()
{
    auto segment = pathSegments.Add();

    return segment;
}

void SettlementAllocator::Free(Settlement *settlement)
{
    settlement->groupDynamics->adventurers.Terminate(adventurerAllocator);

    settlement->groupDynamics->merchants.Terminate(merchantAllocator);

    settlement->groupDynamics->bandits.Terminate(banditAllocator);

    settlement->groupDynamics->patrols.Terminate(patrolAllocator);

    settlement->groupDynamics->garrisons.Terminate(garrisonAllocator);

    settlement->groupDynamics->raiders.Terminate(raiderAllocator);

    groupDynamics.RemoveAt(settlement->groupDynamics);

    auto &tiles = settlement->GetTiles();
    tiles.Terminate(tileAllocator);

    settlement->afflictions.Terminate(afflictionAllocator);

    settlement->events.Terminate(eventAllocator);

    productionAllocator.RemoveAt(settlement->buildingProduction);

    productionAllocator.RemoveAt(settlement->groupProduction);

    ConditionAllocator::Free(conditionAllocator, *settlement->conditionManager);

    conditionManagerAllocator.RemoveAt(settlement->conditionManager);

    ModifierAllocator::Free(modifierAllocator, settlement->modifierManager);

    buildingManagerAllocator.RemoveAt(settlement->buildingManager);

    BuildingSetAllocator::Free(buildingAllocator, *settlement->buildingManager);

    settlement->links.Terminate(linkAllocator);

    ResourceAllocator::Free(resourceAllocator, settlement->resourceHandler);

    PopAllocator::Free(needAllocator, settlement->popHandler);

    settlement->finishedExplorations.Terminate(explorationAllocator);

    settlements.RemoveAt(settlement);
}