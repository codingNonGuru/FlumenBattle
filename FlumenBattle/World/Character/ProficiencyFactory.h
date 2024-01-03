#pragma once

#include "FlumenBattle/World/Character/Proficiency.h"

class Proficiency;

class ProficiencyFactory
{
    static const ProficiencyType & BuildSurvivalSkill()
    {
        static const ProficiencyType type = {ProficiencyClasses::SKILL, "Survival", SkillTypes::SURVIVAL, AbilityTypes::WISDOM};
        return type;
    }

    static const ProficiencyType & BuildPersuasionSkill()
    {
        static const ProficiencyType type = {ProficiencyClasses::SKILL, "Persuasion", SkillTypes::PERSUASION, AbilityTypes::CHARISMA};
        return type;
    }

public:
    static Proficiency BuildPerceptionProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::PERCEPTION, "Perception"};
        return {level, &type};
    }

    static Proficiency BuildReflexSaveProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::SAVE, "Reflex save", SavingThrows::REFLEX_SAVE};
        return {level, &type};
    }

    static Proficiency BuildFortitudeSaveProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::SAVE, "Fortitude save", SavingThrows::FORTITUDE_SAVE};
        return {level, &type};
    }

    static Proficiency BuildWillSaveProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::SAVE, "Will save", SavingThrows::WILL_SAVE};
        return {level, &type};
    }

    static Proficiency BuildSimpleWeaponsProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::WEAPON, "Simple weapons", WeaponClasses::SIMPLE_WEAPONS};
        return {level, &type};
    }

    static Proficiency BuildMartialWeaponsProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::WEAPON, "Martial weapons", WeaponClasses::MARTIAL_WEAPONS};
        return {level, &type};
    }

    static Proficiency BuildAdvancedWeaponsProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::WEAPON, "Advanced weapons", WeaponClasses::ADVANCED_WEAPONS};
        return {level, &type};
    }

    static Proficiency BuildUnarmedCombatProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::WEAPON, "Unarmed combat", WeaponClasses::UNARMED_COMBAT};
        return {level, &type};
    }

    static Proficiency BuildArcaneMagicProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::MAGIC, "Arcane magic", MagicTraditions::ARCANE_MAGIC};
        return {level, &type};
    }

    static Proficiency BuildDivineMagicProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::MAGIC, "Divine magic", MagicTraditions::DIVINE_MAGIC};
        return {level, &type};
    }

    static Proficiency BuildOccultMagicProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::MAGIC, "Occult magic", MagicTraditions::OCCULT_MAGIC};
        return {level, &type};
    }

    static Proficiency BuildPrimalMagicProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::MAGIC, "Primal magic", MagicTraditions::PRIMAL_MAGIC};
        return {level, &type};
    }

    static Proficiency BuildSkillProficiency(SkillTypes type, ProficiencyLevels level)
    {
        const auto skillType = &BuildSkillType(type);
        switch(type)
        {
            case SkillTypes::SURVIVAL:
                return {level, skillType};
            default:
                return {level, nullptr};
        }
    }

    static const ProficiencyType & BuildSkillType(SkillTypes type)
    {
        switch(type)
        {
            case SkillTypes::SURVIVAL:
                return BuildSurvivalSkill();
            case SkillTypes::PERSUASION:
                return BuildPersuasionSkill();
            default:
                return {ProficiencyClasses::SKILL, "None", SkillTypes::NONE, AbilityTypes::STRENGTH};
        }
    }
};