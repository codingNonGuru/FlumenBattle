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

    Weapon() {}

    Weapon(WeaponTypes type) : Type(type) {}

    Weapon(WeaponTypes type, Word name, Integer hitDice, Integer rollCount, Integer range) : 
        Type(type), Name(name), HitDice(hitDice), RollCount(rollCount), Range(range) {}

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