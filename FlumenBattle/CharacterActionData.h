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

    CharacterActionData() : Character(nullptr) {}

    CharacterActionData(CharacterActions actionType, class Character * character, Integer attackRoll, Integer targetArmor, Integer damage)
    {
        ActionType = actionType;
        Character = character;
        AttackRoll = attackRoll;
        TargetArmorClass = targetArmor;
        Damage = damage;
    }

    bool HasHit() {return AttackRoll >= TargetArmorClass;}
};