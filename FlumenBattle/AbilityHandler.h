#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Ability.h"

class AbilityHandler
{
    container::Block <Ability, 6> abilities;

    AbilityTypes attackAbility;

    AbilityTypes spellCastingAbility;

public:
    AbilityHandler()
    {
        for(Index i = 0; i < 6; ++i)
        {
            abilities[i]->Type = (AbilityTypes)i;
        }
    }

    const Ability * GetAbility(AbilityTypes type) const
    {
        auto ability = abilities.Get((int)type);
        return ability;
    }

    Integer GetModifier(AbilityTypes type) const
    {
        return GetAbility(type)->Modifier;
    }

    void SetScore(AbilityTypes type, Integer score)
    {
        auto ability = abilities[(int)type];
        ability->SetScore(score);
    }

    void Boost(AbilityTypes type, Integer amount)
    {
        auto ability = abilities[(int)type];
        ability->SetScore(ability->Score + amount);
    }
    
    const Ability * GetAttackAbility()
    {
        return GetAbility(attackAbility);
    }

    const Ability * GetSpellCastingAbility()
    {
        return GetAbility(spellCastingAbility);
    }

    void SetAttackAbility(AbilityTypes type)
    {
        attackAbility = type;
    }

    void SetSpellCastingAbility(AbilityTypes type)
    {
        spellCastingAbility = type;
    }
};