#pragma once

#include "FlumenBattle/Types.hpp"

class Character;
class Group;
struct Race;
struct CharacterClass;

class CharacterFactory
{
    static Integer GetRandomAbilityScore();

public:
    static Character* Create(const Race *, const CharacterClass *, Group &); 
};