#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

struct Weapon
{
    WeaponClasses Class;

    WeaponTypes Type;

    Word Name;

    Integer HitDice;

    Integer RollCount;

    Integer Range;

    Weapon() {}

    Weapon(WeaponTypes type) : Type(type) {}

    Weapon(WeaponClasses weaponClass, WeaponTypes type, Word name, Integer hitDice, Integer rollCount, Integer range) : 
        Class(weaponClass), Type(type), Name(name), HitDice(hitDice), RollCount(rollCount), Range(range) {}

    bool operator== (const Weapon &other) {return this->Type == other.Type;}

    Integer GetAverageDamage() const 
    {
        auto damage = RollCount + RollCount * HitDice;
        return damage / 2;
    }
};

class WeaponFactory
{
public:
    static Weapon BuildGreatSword() 
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::GREAT_SWORD, "Great sword", 6, 2, 1};
    }

    static Weapon BuildShortSword() 
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::SHORT_SWORD, "Short sword", 6, 1, 1};
    }

    static Weapon BuildHatchet()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::HAND_AXE, "Hatchet", 6, 1, 4};
    }

    static Weapon BuildGreatAxe()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::GREAT_AXE, "Great axe", 12, 1, 1};
    }

    static Weapon BuildShortBow()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::SHORT_BOW, "Short bow", 6, 1, 16};
    }

    static Weapon BuildLongBow()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::LONG_BOW, "Long bow", 8, 1, 30};
    }

    static Weapon BuildMace()
    {
        return {WeaponClasses::SIMPLE_WEAPONS, WeaponTypes::MACE, "Mace", 6, 1, 1};
    }

    static Weapon BuildStaff()
    {
        return {WeaponClasses::SIMPLE_WEAPONS, WeaponTypes::STAFF, "Staff", 4, 1, 1};
    }
};