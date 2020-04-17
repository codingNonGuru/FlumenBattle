#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterFactory.h"

Array <CharacterClasses> groupMakeup = {
    CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, 
    CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::RANGER, 
    CharacterClasses::CLERIC,
    CharacterClasses::WIZARD, CharacterClasses::WIZARD
    };

Group::Group() {}

void Group::Initialize(Integer size, Color color)
{
    this->color = color;

    characters.Initialize(16);

    for(int i = 0; i < size; ++i)
    {
        auto dice = utility::GetRandom(0, groupMakeup.GetSize() - 1);
        CharacterClasses type = *groupMakeup.Get(dice);
        CharacterFactory::Create(type, *this);
    }
}



