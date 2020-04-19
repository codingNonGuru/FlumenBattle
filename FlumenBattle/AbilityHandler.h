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

    Ability * GetAbility(AbilityTypes type)
    {
        return abilities[(int)type];
    }

    Integer GetModifier(AbilityTypes type)
    {
        return abilities[(int)type]->Modifier;
    }

    void SetScore(AbilityTypes type, Integer score)
    {
        GetAbility(type)->SetScore(score);
    }

    void Boost(AbilityTypes type, Integer amount)
    {
        auto ability = GetAbility(type);
        ability->SetScore(ability->Score + amount);
    }
    
    Ability * GetAttackAbility()
    {
        return GetAbility(attackAbility);
    }

    Ability * GetSpellCastingAbility()
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