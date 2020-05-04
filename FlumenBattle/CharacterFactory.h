#pragma once

#include "FlumenBattle/Types.hpp"

class Character;
struct Race;
struct CharacterClass;
namespace world::group
{
    class Group;
}

class CharacterFactory
{
    static Integer GetRandomAbilityScore();

public:
    static Character* Create(const Race *, const CharacterClass *, world::group::Group &); 
};