#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldScene.h"

#define MAXIMUM_SETTLEMENT_COUNT 128

#define MAXIMUM_TILES_PER_SETTLEMENT 19

using namespace world::settlement;

SettlementAllocator::SettlementAllocator()
{
    settlements.Initialize(MAXIMUM_SETTLEMENT_COUNT);

    groupDynamics.Initialize(MAXIMUM_SETTLEMENT_COUNT);

    tileAllocator = container::PoolAllocator <SettlementTile> (MAXIMUM_SETTLEMENT_COUNT, MAXIMUM_TILES_PER_SETTLEMENT);

    afflictionAllocator = container::PoolAllocator <Affliction> (MAXIMUM_SETTLEMENT_COUNT, 16);

    eventAllocator = container::PoolAllocator <SettlementEvent> (MAXIMUM_SETTLEMENT_COUNT, 32);

    productionAllocator = container::Pool <SettlementProduction> (MAXIMUM_SETTLEMENT_COUNT);
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

    return settlement;
}