#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

class Character;

struct CharacterActionData
{
    CharacterActions ActionType;

    class Character * Character;

    class Character * Target;

    Integer AttackRoll;

    Integer TargetArmorClass;

    Integer Damage;

    CharacterActionData() : Character(nullptr) {}

    CharacterActionData(CharacterActions actionType, class Character * character, class Character * target, Integer attackRoll, Integer targetArmor, Integer damage)
    {
        ActionType = actionType;
        Character = character;
        Target = target;
        AttackRoll = attackRoll;
        TargetArmorClass = targetArmor;
        Damage = damage;
    }

    bool HasHit() {return AttackRoll >= TargetArmorClass;}
};