#pragma once

#include "FlumenBattle/Types.hpp"

struct CharacterClass
{
    CharacterClasses Class;

    Word Name;

    Integer HitDice;

    Integer ArmorClass;

    bool IsClass(CharacterClasses _class) const {return Class == _class;}
};