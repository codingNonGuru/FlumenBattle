#pragma once

#include "FlumenBattle/FlumenTypes/Types.hpp"

class Character;
class Group;

class CharacterFactory
{
    static Integer GetRandomAbilityScore();

public:
    static Character* Create(CharacterTypes type, Group& group); 
};