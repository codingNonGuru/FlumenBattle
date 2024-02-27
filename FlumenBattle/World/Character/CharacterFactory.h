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
    struct RecruitData;

    class CharacterFactory
    {
        static Integer GetRandomAbilityScore();

    public:
        static Character* Create(const RecruitData &, world::group::Group &);

        static Character* Create(const Race *, const CharacterClass *, world::group::Group &, int = -1);

        static int GetIconCount(); 

        static const char *GetTextureName(int);
    };
}