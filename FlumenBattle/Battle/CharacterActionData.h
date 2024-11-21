#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Utility/Utility.h"

namespace battle
{
    class Combatant;

    struct SavingThrow
    {
        Combatant *Combatant;

        utility::Success Result;
    };

    struct SpellEffect
    {
        SavingThrow Throw;

        int Damage;
    };

    struct CharacterActionData
    {
        world::character::CharacterActions ActionType;

        class Combatant *Combatant;

        Integer AttackRoll;

        Integer TargetArmorClass;

        Integer Damage;

        bool HasSucceeded;

        bool IsTargetingTile;

        container::Array <SpellEffect> *SpellEffects;

        CharacterActionData() : Combatant(nullptr) {}

        CharacterActionData(world::character::CharacterActions actionType, 
            class Combatant * combatant, 
            Integer attackRoll, 
            Integer targetArmor, 
            Integer damage, 
            bool hasSucceeded,
            container::Array <SpellEffect> *effects = nullptr,
            bool isTargetingTile = false)
        {
            ActionType = actionType;
            Combatant = combatant;
            AttackRoll = attackRoll;
            TargetArmorClass = targetArmor;
            Damage = damage;
            HasSucceeded = hasSucceeded;
            SpellEffects = effects;
            IsTargetingTile = isTargetingTile;
        }
    };
}