#pragma once

#include "FlumenBattle/Types.hpp"

class Character;
class Group;

class CharacterFactory
{
    static Integer GetRandomAbilityScore();

public:
    static Character* Create(CharacterClasses type, Group& group); 
};