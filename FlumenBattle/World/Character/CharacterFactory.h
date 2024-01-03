#pragma once

#include "FlumenBattle/Types.hpp"

struct Race;
namespace world::group
{
    class Group;
}

namespace world::character
{
    class Character;
    struct CharacterClass;

    class CharacterFactory
    {
        static Integer GetRandomAbilityScore();

    public:
        static Character* Create(const Race *, const CharacterClass *, world::group::Group &); 
    };
}