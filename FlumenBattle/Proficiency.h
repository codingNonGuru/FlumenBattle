#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

enum class ProficiencyLevels
{
    UNTRAINED, TRAINED, EXPERT, MASTER, LEGENDARY
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

    ProficiencyIdentifier (SavingThrows savingThrow) {SavingThrow = savingThrow;}

    bool operator== (WeaponClasses weaponClass) {return WeaponClass == weaponClass;}
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

