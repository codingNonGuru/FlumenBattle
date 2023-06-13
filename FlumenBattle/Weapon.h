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

    bool IsThrown;

    bool HasFinesse;

    Weapon() {}

    Weapon(WeaponTypes type) : Type(type) {}

    Weapon(WeaponClasses weaponClass, WeaponTypes type, Word name, Integer hitDice, Integer rollCount, Integer range, bool isThrown, bool hasFinesse) : 
        Class(weaponClass), Type(type), Name(name), HitDice(hitDice), RollCount(rollCount), Range(range), IsThrown(isThrown), HasFinesse(hasFinesse) {}

    bool operator== (const Weapon &other) {return this->Type == other.Type;}

    bool operator== (WeaponTypes type) {return this->Type == type;}

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
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::GREAT_SWORD, "Great sword", 6, 2, 1, false, false};
    }

    static Weapon BuildShortSword() 
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::SHORT_SWORD, "Short sword", 6, 1, 1, false, false};
    }

    static Weapon BuildHatchet()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::HAND_AXE, "Hatchet", 6, 1, 4, true, false};
    }

    static Weapon BuildGreatAxe()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::GREAT_AXE, "Great axe", 12, 1, 1, false, false};
    }

    static Weapon BuildShortBow()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::SHORT_BOW, "Short bow", 6, 1, 16, false, false};
    }

    static Weapon BuildLongBow()
    {
        return {WeaponClasses::MARTIAL_WEAPONS, WeaponTypes::LONG_BOW, "Long bow", 8, 1, 30, false, false};
    }

    static Weapon BuildMace()
    {
        return {WeaponClasses::SIMPLE_WEAPONS, WeaponTypes::MACE, "Mace", 6, 1, 1, false, false};
    }

    static Weapon BuildStaff()
    {
        return {WeaponClasses::SIMPLE_WEAPONS, WeaponTypes::STAFF, "Staff", 4, 1, 1, false, false};
    }
};