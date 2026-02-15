#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;
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

    const char *TextureName;

    Integer HitPointBonus;

    Integer Speed;

    AbilityBoost Boosts[ABILITY_COUNT];

    container::Array <world::settlement::ResourceTypes> ProductionAffinities;

    Race(RaceTypes type, const char *name, const char *pluralName, const char *textureName, Integer bonus, Integer speed, std::initializer_list <AbilityBoost> boosts) : 
        Type(type), Name(name), PluralName(pluralName), TextureName(textureName), HitPointBonus(bonus), Speed(speed) 
        {
            auto i = 0;
            for(auto &boost : boosts)
            {
                Boosts[i] = boost;
                i++;
            }
        }

    Race(RaceTypes type, const char *name, const char *pluralName, const char *textureName, Integer bonus, Integer speed, std::initializer_list <AbilityBoost> boosts, std::initializer_list <world::settlement::ResourceTypes> productionAffinities) : 
        Type(type), Name(name), PluralName(pluralName), TextureName(textureName), HitPointBonus(bonus), Speed(speed), ProductionAffinities(productionAffinities)
        {
            auto i = 0;
            for(auto &boost : boosts)
            {
                Boosts[i] = boost;
                i++;
            }
        }

    virtual void ApplyModifiers(world::settlement::Settlement &) const {}

    bool HasAffinityFor(world::settlement::ResourceTypes resource) const 
    {
        return ProductionAffinities.Find(resource) != nullptr;
    }
};
