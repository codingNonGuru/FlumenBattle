#include "RaceBehavior.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

void Dwarf::ApplyModifiers(world::settlement::Settlement &settlement) const
{
    settlement.AddModifier({world::settlement::Modifiers::SAVING_THROWS_AGAINST_DISEASE, 1});
    settlement.AddModifier({world::settlement::Modifiers::PRODUCTION_FROM_METAL_RESOURCE, 1});
    settlement.AddModifier({world::settlement::Modifiers::COMBAT_BONUS_ON_MOUNTAIN_TILES, 1});
    settlement.AddModifier({world::settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES, 1});
    settlement.AddModifier({world::settlement::Modifiers::DURATION_BETWEEN_TRADE_SHIPMENTS, 3});
}

void Elf::ApplyModifiers(world::settlement::Settlement &settlement) const
{
    settlement.AddModifier({world::settlement::Modifiers::FOOD_PRODUCTION_FROM_WOOD_TILES, 1});
    settlement.AddModifier({world::settlement::Modifiers::COMBAT_BONUS_ON_WOOD_TILES, 1});
}

void Goblin::ApplyModifiers(world::settlement::Settlement &settlement) const
{
    settlement.AddModifier({world::settlement::Modifiers::POPULATION_GROWTH_RATE, 1});
}