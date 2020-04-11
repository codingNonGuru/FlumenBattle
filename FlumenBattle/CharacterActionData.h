#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;

struct CharacterActionData
{
    class Character * Character;

    class Character * Target;

    Integer AttackRoll;

    Integer TargetArmorClass;

    Integer Damage;

    CharacterActionData() : Character(nullptr) {}

    CharacterActionData(class Character * character, class Character * target, Integer attackRoll, Integer targetArmor, Integer damage)
    {
        Character = character;
        Target = target;
        AttackRoll = attackRoll;
        TargetArmorClass = targetArmor;
        Damage = damage;
    }

    bool HasHit() {return AttackRoll >= TargetArmorClass;}
};