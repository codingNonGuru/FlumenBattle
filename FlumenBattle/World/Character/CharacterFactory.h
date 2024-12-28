#pragma once

#include "FlumenBattle/Types.hpp"

struct Race;
namespace world::group
{
    class GroupCore;
    struct CharacterEssence;
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
        static Character* Create(const RecruitData &, world::group::GroupCore &);

        static Character* Create(const Race *, const CharacterClass *, world::group::GroupCore &, int = -1);

        static Character* Create(const Race *, const group::CharacterEssence &, world::group::GroupCore &, int = -1);

        static int GetIconCount(); 

        static const char *GetTextureName(int);
    };
}