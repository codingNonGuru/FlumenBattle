#include "FlumenBattle/World/Polity/PolityAllocator.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/World/Polity/Polity.h"

using namespace world::polity;

#define FACTION_MEMBERS_COUNT 16

#define POLITY_FACTION_COUNT 16

void PolityAllocator::PreallocateMaximumMemory()
{
    auto worldGenerator = WorldGenerator::Get();

    auto polityCount = worldGenerator->GetMaximumPolityCount(MAXIMUM_WORLD_SIZE);

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(MAXIMUM_WORLD_SIZE);

    polityMemory = container::Pool <Polity>::PreallocateMemory(polityCount);

    politySettlementMemory = container::PoolAllocator <settlement::Settlement *>::PreallocateMemory(polityCount, settlementCount);

    factionMemory = container::PoolAllocator <Faction>::PreallocateMemory(polityCount, POLITY_FACTION_COUNT);

    factionSettlementMemory = container::PoolAllocator <settlement::Settlement *>::PreallocateMemory(polityCount * POLITY_FACTION_COUNT, FACTION_MEMBERS_COUNT);
}

void PolityAllocator::AllocateWorldMemory(int worldSize)
{
    auto worldGenerator = WorldGenerator::Get();

    auto polityCount = worldGenerator->GetMaximumPolityCount(worldSize);

    auto settlementCount = worldGenerator->GetMaximumSettlementCount(worldSize);

    polities.Initialize(polityCount, polityMemory);

    politySettlementAllocator = container::PoolAllocator <settlement::Settlement *> (polityCount, settlementCount, politySettlementMemory);

    factionAllocator = container::PoolAllocator <Faction> (polityCount, POLITY_FACTION_COUNT, factionMemory);

    factionSettlementAllocator = container::PoolAllocator <settlement::Settlement *> (polityCount * POLITY_FACTION_COUNT, FACTION_MEMBERS_COUNT, factionSettlementMemory);
}

Polity *PolityAllocator::AllocatePolity()
{
    auto polity = polities.Add();

    polity->factions.Initialize(factionAllocator);

    polity->settlements.Initialize(politySettlementAllocator);

    return polity;
}

Faction *PolityAllocator::AllocateFaction(Polity *polity)
{
    auto faction = polity->factions.Add();

    faction->members.Initialize(factionSettlementAllocator);

    return faction;
}

void PolityAllocator::FreeFaction(Polity *polity, Faction *faction)
{
    polity->factions.RemoveAt(faction);

    faction->members.Terminate(factionSettlementAllocator);
}