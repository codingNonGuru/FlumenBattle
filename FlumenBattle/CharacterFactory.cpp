#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"

Character* CharacterFactory::Create(CharacterTypes type, Group& group)
{
    auto character = group.characters.Allocate();

    character->type = type;
    character->position = Position2(utility::GetRandom(-500.0f, 500.0f), utility::GetRandom(-500.0f, 500.0f));

    return character;
}