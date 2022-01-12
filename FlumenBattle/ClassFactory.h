#pragma once

#include "FlumenBattle/CharacterClass.h"

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
        static CharacterClass fighterClass = {CharacterClasses::FIGHTER, "Fighter", 10, 16};
        return fighterClass;
    }
 
    static const CharacterClass & BuildRangerClass()
    {
        static CharacterClass rangerClass = {CharacterClasses::RANGER, "Ranger", 10, 14};
        return rangerClass;
    }

    static const CharacterClass & BuildClericClass()
    {
        static CharacterClass clericClass = {CharacterClasses::CLERIC, "Cleric", 8, 12};
        return clericClass;
    }

    static const CharacterClass & BuildWizardClass()
    {
        static CharacterClass wizardClass = {CharacterClasses::WIZARD, "Wizard", 6, 10};
        return wizardClass;
    }
};