#pragma once

#include "FlumenBattle/Types.hpp"

class Character;
class Group;
struct Race;

class CharacterFactory
{
    static Integer GetRandomAbilityScore();

public:
    static Character* Create(const Race *, CharacterClasses, Group &); 
};