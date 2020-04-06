#include "Group.h"
#include "Character.h"
#include "CharacterFactory.h"
#include "BattleTypes.hpp"

void Group::Initialize()
{
    characters.Initialize(MAXIMUM_TROOP_COUNT);

    CharacterFactory::Create(CharacterTypes::FIGHTER, *this);
    CharacterFactory::Create(CharacterTypes::RANGER, *this);
    CharacterFactory::Create(CharacterTypes::FIGHTER, *this);
    CharacterFactory::Create(CharacterTypes::RANGER, *this);
    CharacterFactory::Create(CharacterTypes::CLERIC, *this);
}

