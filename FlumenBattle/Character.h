#pragma once

#include "FlumenBattle/FlumenTypes/Types.hpp"

#include "Types.hpp"

class CharacterFactory;

class Character
{
    Position2 position;

    CharacterTypes type;

    Integer level;

    float skill;

    Character();

    friend class CharacterFactory;
    friend class CharacterInfo;
};