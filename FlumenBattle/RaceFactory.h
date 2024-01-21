#pragma once

#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/Types.h"

class RaceFactory
{
public:
    static const Race * BuildRandomRace()
    {
        RaceTypes index = (RaceTypes)utility::GetRandom(0, 6);
        switch(index)
        {
            case RaceTypes::DWARF:
                return BuildDwarfRace();
            case RaceTypes::ELF:
                return BuildElfRace();
            case RaceTypes::HUMAN:
                return BuildHumanRace();
            case RaceTypes::HALFLING:
                return BuildHalflingRace();
            case RaceTypes::GNOME:
                return BuildGnomeRace();
            case RaceTypes::GOBLIN:
                return BuildGoblinRace();
            case RaceTypes::ORC:
                return BuildOrcRace();
        }
    }  

    static const Race * BuildRace(RaceTypes type)
    {
        switch(type)
        {
            case RaceTypes::DWARF:
                return BuildDwarfRace();
            case RaceTypes::ELF:
                return BuildElfRace();
            case RaceTypes::HUMAN:
                return BuildHumanRace();
            case RaceTypes::HALFLING:
                return BuildHalflingRace();
            case RaceTypes::GNOME:
                return BuildGnomeRace();
            case RaceTypes::GOBLIN:
                return BuildGoblinRace();
            case RaceTypes::ORC:
                return BuildOrcRace();
        }
    } 

    static const Race * BuildDwarfRace()
    {
        static Race race = {RaceTypes::DWARF, "Dwarf", 10, 4,
            {
                {world::character::AbilityTypes::STRENGTH, 1}, 
                {world::character::AbilityTypes::DEXTERITY, -1}, 
                {world::character::AbilityTypes::CONSTITUTION, 2}, 
                {world::character::AbilityTypes::WISDOM, 1},
                {world::character::AbilityTypes::CHARISMA, -1}
            }
        };
        return &race;
    }

    static const Race * BuildElfRace()
    {
        static Race race = {RaceTypes::ELF, "Elf", 6, 6,
            {
                {world::character::AbilityTypes::DEXTERITY, 2}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CONSTITUTION, -1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            }
        };
        return &race;
    }

    static const Race * BuildHumanRace()
    {
        static Race race = {RaceTypes::HUMAN, "Human", 8, 5,
            {
                {world::character::AbilityTypes::DEXTERITY, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            }
        };
        return &race;
    }

    static const Race * BuildGnomeRace()
    {
        static Race race = {RaceTypes::GNOME, "Gnome", 8, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::CONSTITUTION, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            }
        };
        return &race;
    }

    static const Race * BuildHalflingRace()
    {
        static Race race = {RaceTypes::HALFLING, "Halfling", 6, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::DEXTERITY, 2}, 
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            }
        };
        return &race;
    }

    static const Race * BuildGoblinRace()
    {
        static Race race = {RaceTypes::GOBLIN, "Goblin", 6, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::DEXTERITY, 2},
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, -1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            }
        };
        return &race;
    }

    static const Race * BuildOrcRace()
    {
        static Race race = {RaceTypes::ORC, "Orc", 10, 5,
            {
                {world::character::AbilityTypes::STRENGTH, 2}, 
                {world::character::AbilityTypes::CONSTITUTION, 2},
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, -1}, 
                {world::character::AbilityTypes::CHARISMA, -1}
            }
        };
        return &race;
    }
};