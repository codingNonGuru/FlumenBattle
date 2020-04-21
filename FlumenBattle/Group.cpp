#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/ClassFactory.h"

Array <CharacterClasses> classMakeup; /*= {
    CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, 
    CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::RANGER, 
    CharacterClasses::CLERIC,
    CharacterClasses::WIZARD, CharacterClasses::WIZARD
    };*/

Group::Group() {}

void Group::Initialize(Integer size, Color color, RaceTypes raceType)
{
    this->color = color;

    characters.Initialize(16);

    for(int i = 0; i < size; ++i)
    {
        switch(raceType)
        {
            case RaceTypes::DWARF:
                classMakeup = {CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::CLERIC};
                break;
            case RaceTypes::HUMAN:
                classMakeup = {CharacterClasses::FIGHTER, CharacterClasses::CLERIC, CharacterClasses::RANGER, CharacterClasses::WIZARD};
                break;
            case RaceTypes::ELF:
                classMakeup = {CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::WIZARD};
                break;
            case RaceTypes::GNOME:
                classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC, CharacterClasses::WIZARD, CharacterClasses::WIZARD};
                break;
            case RaceTypes::HALFLING:
                classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC};
                break;
            case RaceTypes::GOBLIN:
                classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC};
                break;
            case RaceTypes::ORC:
                classMakeup = {CharacterClasses::FIGHTER};
                break;
        }

        auto dice = utility::GetRandom(0, classMakeup.GetSize() - 1);
        CharacterClasses type = *classMakeup.Get(dice);

        auto race = RaceFactory::BuildRace(raceType);

        CharacterFactory::Create(race, &ClassFactory::BuildClass(type), *this);
    }
}



