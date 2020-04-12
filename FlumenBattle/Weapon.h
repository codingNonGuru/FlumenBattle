#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

struct Weapon
{
    WeaponTypes Type;

    Integer HitDice;

    Integer RollCount;

    Integer Range;
};

class WeaponFactory
{
public:
    static Weapon BuildSword() 
    {
        return {WeaponTypes::SWORD, 8, 1, 1};
    }

    static Weapon BuildGreataxe()
    {
        return {WeaponTypes::GREAT_AXE, 12, 1, 1};
    }

    static Weapon BuildShortbow()
    {
        return {WeaponTypes::SHORT_BOW, 6, 1, 10};
    }

    static Weapon BuildLongbow()
    {
        return {WeaponTypes::LONG_BOW, 8, 1, 15};
    }

    static Weapon BuildMace()
    {
        return {WeaponTypes::MACE, 6, 1, 1};
    }

    static Weapon BuildStaff()
    {
        return {WeaponTypes::STAFF, 4, 1, 1};
    }
};