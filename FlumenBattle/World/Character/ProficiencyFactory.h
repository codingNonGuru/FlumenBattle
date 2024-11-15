#pragma once

#include "FlumenBattle/World/Character/Proficiency.h"
#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
    class Proficiency;

    class ProficiencyFactory
    {
        static const ProficiencyType & BuildSurvivalSkill()
        {
            static const ProficiencyType type = {CheckClasses::SKILL, "Survival", SkillTypes::SURVIVAL, AbilityTypes::WISDOM};
            return type;
        }

        static const ProficiencyType & BuildPersuasionSkill()
        {
            static const ProficiencyType type = {CheckClasses::SKILL, "Persuasion", SkillTypes::PERSUASION, AbilityTypes::CHARISMA};
            return type;
        }

        static const ProficiencyType & BuildPerceptionSkill()
        {
            static const ProficiencyType type = {CheckClasses::SKILL, "Perception", SkillTypes::PERCEPTION, AbilityTypes::WISDOM};
            return type;
        }

        static const ProficiencyType & BuildStealthSkill()
        {
            static const ProficiencyType type = {CheckClasses::SKILL, "Stealth", SkillTypes::STEALTH, AbilityTypes::DEXTERITY};
            return type;
        }

    public:
        static Proficiency BuildReflexSaveProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::SAVE, "Reflex save", SavingThrows::REFLEX};
            return {level, &type};
        }

        static Proficiency BuildFortitudeSaveProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::SAVE, "Fortitude save", SavingThrows::FORTITUDE};
            return {level, &type};
        }

        static Proficiency BuildWillSaveProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::SAVE, "Will save", SavingThrows::WILL};
            return {level, &type};
        }

        static Proficiency BuildSimpleWeaponsProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::WEAPON, "Simple weapons", WeaponClasses::SIMPLE_WEAPONS};
            return {level, &type};
        }

        static Proficiency BuildMartialWeaponsProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::WEAPON, "Martial weapons", WeaponClasses::MARTIAL_WEAPONS};
            return {level, &type};
        }

        static Proficiency BuildAdvancedWeaponsProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::WEAPON, "Advanced weapons", WeaponClasses::ADVANCED_WEAPONS};
            return {level, &type};
        }

        static Proficiency BuildUnarmedCombatProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::WEAPON, "Unarmed combat", WeaponClasses::UNARMED_COMBAT};
            return {level, &type};
        }

        static Proficiency BuildArcaneMagicProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::MAGIC, "Arcane magic", MagicTraditions::ARCANE_MAGIC};
            return {level, &type};
        }

        static Proficiency BuildDivineMagicProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::MAGIC, "Divine magic", MagicTraditions::DIVINE_MAGIC};
            return {level, &type};
        }

        static Proficiency BuildOccultMagicProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::MAGIC, "Occult magic", MagicTraditions::OCCULT_MAGIC};
            return {level, &type};
        }

        static Proficiency BuildPrimalMagicProficiency(ProficiencyLevels level)
        {
            static ProficiencyType type = {CheckClasses::MAGIC, "Primal magic", MagicTraditions::PRIMAL_MAGIC};
            return {level, &type};
        }

        static Proficiency BuildSkillProficiency(SkillTypes type, ProficiencyLevels level)
        {
            const auto skillType = &BuildSkillType(type);
            switch(type)
            {
                case SkillTypes::SURVIVAL:
                    return {level, skillType};
                case SkillTypes::PERSUASION:
                    return {level, skillType};
                case SkillTypes::PERCEPTION:
                    return {level, skillType};
                case SkillTypes::STEALTH:
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
                case SkillTypes::PERCEPTION:
                    return BuildPerceptionSkill();
                case SkillTypes::STEALTH:
                    return BuildStealthSkill();
                default:
                    return {CheckClasses::SKILL, "None", SkillTypes::NONE, AbilityTypes::STRENGTH};
            }
        }
    };
}