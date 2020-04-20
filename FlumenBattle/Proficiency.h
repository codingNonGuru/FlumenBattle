#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

enum class ProficiencyLevels
{
    UNTRAINED, APPRENTICE, TRAINED, EXPERIENCED, MASTER, LEGENDARY
};

enum class ProficiencyClasses
{
    SAVE, PERCEPTION, WEAPON, ARMOR, MAGIC, SKILL
};

union ProficiencyIdentifier
{
    MagicTraditions MagicTradition;

    ArmorClasses ArmorClass;

    WeaponClasses WeaponClass;

    SkillTypes SkillType;

    SavingThrows SavingThrow;

    ProficiencyIdentifier() {}

    ProficiencyIdentifier (WeaponClasses weaponClass) {WeaponClass = weaponClass;}

    ProficiencyIdentifier (SavingThrows savingThrow) {SavingThrow = savingThrow;}

    ProficiencyIdentifier (MagicTraditions magicTradition) {MagicTradition = magicTradition;}

    bool operator== (WeaponClasses weaponClass) const {return WeaponClass == weaponClass;}

    bool operator== (SavingThrows savingThrow) const {return SavingThrow == savingThrow;}
};

struct ProficiencyType
{
    ProficiencyClasses Class;

    Word Name;

    ProficiencyIdentifier Identifier;
};

struct Proficiency
{
    ProficiencyLevels Level;

    const ProficiencyType *Type;

    Proficiency() : Level(ProficiencyLevels::UNTRAINED), Type(nullptr) {}

    Proficiency(ProficiencyLevels level, const ProficiencyType *type) : Level(level), Type(type) {}

    void operator= (const Proficiency &other) 
    {
        this->Level = other.Level;
        this->Type = other.Type;
    }
};

