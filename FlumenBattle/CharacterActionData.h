#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

class Combatant;

struct CharacterActionData
{
    CharacterActions ActionType;

    class Combatant * Combatant;

    Integer AttackRoll;

    Integer TargetArmorClass;

    Integer Damage;

    bool HasSucceeded;

    CharacterActionData() : Combatant(nullptr) {}

    CharacterActionData(CharacterActions actionType, class Combatant * combatant, Integer attackRoll, Integer targetArmor, Integer damage, bool hasSucceeded)
    {
        ActionType = actionType;
        Combatant = combatant;
        AttackRoll = attackRoll;
        TargetArmorClass = targetArmor;
        Damage = damage;
        HasSucceeded = hasSucceeded;
    }
};