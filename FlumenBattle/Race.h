#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"

namespace world::settlement
{
    class Settlement;
}

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

    const char *Name;

    const char *PluralName;

    Integer HitPointBonus;

    Integer Speed;

    AbilityBoost Boosts[ABILITY_COUNT];

    Race(RaceTypes type, const char *name, const char *pluralName, Integer bonus, Integer speed, std::initializer_list <AbilityBoost> boosts) : 
        Type(type), Name(name), PluralName(pluralName), HitPointBonus(bonus), Speed(speed) 
        {
            auto i = 0;
            for(auto &boost : boosts)
            {
                Boosts[i] = boost;
                i++;
            }
        }

    virtual void ApplyModifiers(world::settlement::Settlement &) const {}
};
