#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
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

        ProficiencyIdentifier (SkillTypes skillType) {SkillType = skillType;}

        bool operator== (WeaponClasses weaponClass) const {return WeaponClass == weaponClass;}

        bool operator== (SavingThrows savingThrow) const {return SavingThrow == savingThrow;}

        bool operator== (SkillTypes skillType) const {return SkillType == skillType;}
    };

    struct ProficiencyType
    {
        ProficiencyClasses Class;

        Word Name;

        ProficiencyIdentifier Identifier;

        AbilityTypes AssociatedAbility;

        ProficiencyType(ProficiencyClasses proficiencyClass, Word name, ProficiencyIdentifier identifier, AbilityTypes ability) :
            Class(proficiencyClass), Name(name), Identifier(identifier), AssociatedAbility(ability) {}

        ProficiencyType(ProficiencyClasses proficiencyClass, Word name, ProficiencyIdentifier identifier) :
            Class(proficiencyClass), Name(name), Identifier(identifier) {}

        ProficiencyType(ProficiencyClasses proficiencyClass, Word name) :
            Class(proficiencyClass), Name(name) {}
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
}