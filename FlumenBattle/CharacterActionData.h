#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

class Character;

struct CharacterActionData
{
    CharacterActions ActionType;

    class Character * Character;

    Integer AttackRoll;

    Integer TargetArmorClass;

    Integer Damage;

    bool HasSucceeded;

    CharacterActionData() : Character(nullptr) {}

    CharacterActionData(CharacterActions actionType, class Character * character, Integer attackRoll, Integer targetArmor, Integer damage, bool hasSucceeded)
    {
        ActionType = actionType;
        Character = character;
        AttackRoll = attackRoll;
        TargetArmorClass = targetArmor;
        Damage = damage;
        HasSucceeded = hasSucceeded;
    }
};