#pragma once

#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/Spell.h"

namespace world::character
{
    class ClassFactory //Possible classes: Fighter, Wizard, Ranger, Cleric, Bard, Barbarian, Alchemist, Champion, Rogue, Monk, Druid, Sorcerer
    {
    public:
        static const CharacterClass & BuildClass(CharacterClasses _class)
        {
            switch(_class)
            {
                case CharacterClasses::FIGHTER:
                    return ClassFactory::BuildFighterClass();
                case CharacterClasses::WIZARD:
                    return ClassFactory::BuildWizardClass();
                case CharacterClasses::RANGER:
                    return ClassFactory::BuildRangerClass();
                case CharacterClasses::CLERIC:
                    return ClassFactory::BuildClericClass();
            }
        }

        static const CharacterClass & BuildFighterClass()
        {
            static CharacterClass fighterClass = 
            {
                CharacterClasses::FIGHTER, 
                "Fighter", 
                10, 
                16, 
                {"Fighter1", Position2(0.0f, 0.0f)},
                {{CharacterActions::ATTACK}, {CharacterActions::DODGE}, {CharacterActions::DASH}}
            };
            return fighterClass;
        }
    
        static const CharacterClass & BuildRangerClass()
        {
            static CharacterClass rangerClass = 
            {
                CharacterClasses::RANGER, 
                "Ranger", 
                10, 
                14, 
                {"Ranger1", Position2(0.75f, 0.0f)},
                {CharacterActions::ATTACK, CharacterActions::DODGE, CharacterActions::DASH}
            };
            return rangerClass;
        }

        static const CharacterClass & BuildClericClass()
        {
            static CharacterClass clericClass = 
            {
                CharacterClasses::CLERIC, 
                "Cleric", 
                8, 
                12, 
                {"Cleric1", Position2(0.5f, 0.0f)},
                {CharacterActions::ATTACK, CharacterActions::DODGE, CharacterActions::DASH, CharacterActions::CAST_SPELL},
                {
                    AbilityTypes::WISDOM, 
                    {SpellFactory::BuildCureWounds(), SpellFactory::BuildSacredFlame(), SpellFactory::BuildHealingWord(), SpellFactory::BuildBless()}
                }
            };
            return clericClass;
        }

        static const CharacterClass & BuildWizardClass()
        {
            static CharacterClass wizardClass = 
            {
                CharacterClasses::WIZARD, 
                "Wizard", 
                6, 
                10, 
                {"Wizard1", Position2(0.0f, 0.5f)},
                {CharacterActions::ATTACK, CharacterActions::DODGE, CharacterActions::DASH, CharacterActions::CAST_SPELL},
                {
                    AbilityTypes::INTELLIGENCE, 
                    {SpellFactory::BuildFrostRay(), SpellFactory::BuildShockingGrasp(), SpellFactory::BuildFireBolt(), SpellFactory::BuildMagicMissile()}
                }
            };
            return wizardClass;
        }
    };
}