#pragma once

#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
    struct CharacterClass
    {
        CharacterClasses Class;

        Word Name;

        Integer HitDice;

        Integer ArmorClass;

        Array <CharacterAction> Actions;

        Array <Spell> Spells;

        bool IsClass(CharacterClasses _class) const {return Class == _class;}
    };
}