#pragma once

#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Race.h"
#include "FlumenBattle/RaceBehavior.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"

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
        static Dwarf race = {RaceTypes::DWARF, "Dwarf", "Dwarves", "RaceDwarfHead", 10, 4,
            {
                {world::character::AbilityTypes::STRENGTH, 1}, 
                {world::character::AbilityTypes::DEXTERITY, -1}, 
                {world::character::AbilityTypes::CONSTITUTION, 2}, 
                {world::character::AbilityTypes::WISDOM, 1},
                {world::character::AbilityTypes::CHARISMA, -1}
            },
            {world::settlement::ResourceTypes::ORE, world::settlement::ResourceTypes::POTTERY}
        };
        return &race;
    }

    static const Race * BuildElfRace()
    {
        static Elf race = {RaceTypes::ELF, "Elf", "Elves", "RaceElfHead", 6, 6,
            {
                {world::character::AbilityTypes::DEXTERITY, 2}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CONSTITUTION, -1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            },
            {world::settlement::ResourceTypes::LUMBER, world::settlement::ResourceTypes::FURNITURE, world::settlement::ResourceTypes::CLOTHING}
        };
        return &race;
    }

    static const Race * BuildHumanRace()
    {
        static Human race = {RaceTypes::HUMAN, "Human", "Humans", "RaceHumanHead", 8, 5,
            {
                {world::character::AbilityTypes::DEXTERITY, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            },
            {world::settlement::ResourceTypes::FOOD, world::settlement::ResourceTypes::FIBER, world::settlement::ResourceTypes::POTTERY}
        };
        return &race;
    }

    static const Race * BuildGnomeRace()
    {
        static Race race = {RaceTypes::GNOME, "Gnome", "Gnomes", "RaceGnomeHead", 8, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::CONSTITUTION, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            },
            {world::settlement::ResourceTypes::CLOTHING, world::settlement::ResourceTypes::FURNITURE}
        };
        return &race;
    }

    static const Race * BuildHalflingRace()
    {
        static Race race = {RaceTypes::HALFLING, "Halfling", "Halflings", "RaceHalflingHead", 6, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::DEXTERITY, 2}, 
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            },
            {world::settlement::ResourceTypes::FABRIC}
        };
        return &race;
    }

    static const Race * BuildGoblinRace()
    {
        static Goblin race = {RaceTypes::GOBLIN, "Goblin", "Goblins", "RaceGoblinHead", 6, 5,
            {
                {world::character::AbilityTypes::STRENGTH, -1}, 
                {world::character::AbilityTypes::DEXTERITY, 2},
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, -1}, 
                {world::character::AbilityTypes::CHARISMA, 1}
            },
            {world::settlement::ResourceTypes::COOKED_FOOD}
        };
        return &race;
    }

    static const Race * BuildOrcRace()
    {
        static Orc race = {RaceTypes::ORC, "Orc", "Orcs", "RaceOrcHead", 10, 5,
            {
                {world::character::AbilityTypes::STRENGTH, 2}, 
                {world::character::AbilityTypes::CONSTITUTION, 2},
                {world::character::AbilityTypes::WISDOM, 1}, 
                {world::character::AbilityTypes::INTELLIGENCE, -1}, 
                {world::character::AbilityTypes::CHARISMA, -1}
            },
            {world::settlement::ResourceTypes::ORE, world::settlement::ResourceTypes::CLAY, world::settlement::ResourceTypes::TIMBER}
        };
        return &race;
    }
};