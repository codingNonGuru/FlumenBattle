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
        CharacterTypes type;
        switch(dice)
        {
        case 0:
            type = CharacterTypes::FIGHTER;
            break;
        case 1:
            type = CharacterTypes::RANGER;
            break;
        case 2:
            type = CharacterTypes::CLERIC;
            break;
        case 3:
            type = CharacterTypes::WIZARD;
            break;
        }

        CharacterFactory::Create(type, *this);
    }
}



