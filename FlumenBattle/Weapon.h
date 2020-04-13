#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

struct Weapon
{
    WeaponTypes Type;

    Word Name;

    Integer HitDice;

    Integer RollCount;

    Integer Range;
};

class WeaponFactory
{
public:
    static Weapon BuildGreatSword() 
    {
        return {WeaponTypes::GREAT_SWORD, "Great sword", 6, 2, 1};
    }

    static Weapon BuildShortSword() 
    {
        return {WeaponTypes::SHORT_SWORD, "Short sword", 6, 1, 1};
    }

    static Weapon BuildHandAxe()
    {
        return {WeaponTypes::HAND_AXE, "Hand axe", 6, 1, 4};
    }

    static Weapon BuildGreatAxe()
    {
        return {WeaponTypes::GREAT_AXE, "Great axe", 12, 1, 1};
    }

    static Weapon BuildShortBow()
    {
        return {WeaponTypes::SHORT_BOW, "Short bow", 6, 1, 16};
    }

    static Weapon BuildLongBow()
    {
        return {WeaponTypes::LONG_BOW, "Long bow", 8, 1, 30};
    }

    static Weapon BuildMace()
    {
        return {WeaponTypes::MACE, "Mace", 6, 1, 1};
    }

    static Weapon BuildStaff()
    {
        return {WeaponTypes::STAFF, "Staff", 4, 1, 1};
    }
};