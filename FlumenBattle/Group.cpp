#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterFactory.h"

Group::Group() {}

void Group::Initialize(Integer size, Color color)
{
    this->color = color;

    characters.Initialize(16);

    for(int i = 0; i < size; ++i)
    {
        auto dice = utility::GetRandom(0, 3);
        CharacterClasses type;
        switch(dice)
        {
        case 0:
            type = CharacterClasses::FIGHTER;
            break;
        case 1:
            type = CharacterClasses::RANGER;
            break;
        case 2:
            type = CharacterClasses::CLERIC;
            break;
        case 3:
            type = CharacterClasses::WIZARD;
            break;
        }

        CharacterFactory::Create(type, *this);
    }
}



