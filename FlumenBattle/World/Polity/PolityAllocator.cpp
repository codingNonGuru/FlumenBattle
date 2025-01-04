#include "FlumenBattle/World/Polity/PolityAllocator.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/Config.h"

using namespace world::polity;

#define FACTION_MEMBERS_COUNT 16

#define POLITY_FACTION_COUNT 16

void PolityAllocator::PreallocateMaximumMemory()
{
    static const auto MAXIMUM_WORLD_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WORLD_SIZE).Integer;

    auto worldGenerator = WorldGenerator::Get();

    auto polityCount = worldGenerator->GetMaximumPolityCount(MAXIMUM_WORLD_SIZE);

    polityMemory = container::Pool <Polity>::PreallocateMemory(polityCount);

    static const auto MAXIMUM_POLITY_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_POLITY_SIZE).Integer;

    politySettlementMemory = container::PoolAllocator <settlement::Settlement *>::PreallocateMemory(polityCount, MAXIMUM_POLITY_SIZE);

    factionMemory = container::PoolAllocator <Faction>::PreallocateMemory(polityCount, POLITY_FACTION_COUNT);

    factionSettlementMemory = container::PoolAllocator <settlement::Settlement *>::PreallocateMemory(polityCount * POLITY_FACTION_COUNT, FACTION_MEMBERS_COUNT);
}

void PolityAllocator::AllocateWorldMemory(int worldSize)
{
    auto worldGenerator = WorldGenerator::Get();

    auto polityCount = worldGenerator->GetMaximumPolityCount(worldSize);

    static const auto MAXIMUM_POLITY_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_POLITY_SIZE).Integer;

    polities.Initialize(polityCount, polityMemory);

    politySettlementAllocator = container::PoolAllocator <settlement::Settlement *> (polityCount, MAXIMUM_POLITY_SIZE, politySettlementMemory);

    factionAllocator = container::PoolAllocator <Faction> (polityCount, POLITY_FACTION_COUNT, factionMemory);

    factionSettlementAllocator = container::PoolAllocator <settlement::Settlement *> (polityCount * POLITY_FACTION_COUNT, FACTION_MEMBERS_COUNT, factionSettlementMemory);
}

static int availableUniqueId = 0;

Polity *PolityAllocator::AllocatePolity()
{
    auto polity = polities.Add();

    polity->uniqueId = availableUniqueId;
    availableUniqueId++;

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

void PolityAllocator::FreePolity(Polity *polity)
{
    for(auto &faction : polity->GetFactions())
    {
        faction.members.Terminate(factionSettlementAllocator);
    }

    polity->factions.Terminate(factionAllocator);

    polity->settlements.Terminate(politySettlementAllocator);

    polities.RemoveAt(polity);
}

void PolityAllocator::FreeFaction(Polity *polity, Faction *faction)
{
    polity->factions.RemoveAt(faction);

    faction->members.Terminate(factionSettlementAllocator);
}