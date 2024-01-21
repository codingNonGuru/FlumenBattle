#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"

struct AbilityBoost
{
    world::character::AbilityTypes Type;

    Integer Boost;

    AbilityBoost() : Boost(0) {}

    AbilityBoost(world::character::AbilityTypes type, Integer boost) : Type(type), Boost(boost) {}
};

struct Race
{
    RaceTypes Type;

    Word Name;

    Integer HitPointBonus;

    Integer Speed;

    AbilityBoost Boosts[ABILITY_COUNT];
};
