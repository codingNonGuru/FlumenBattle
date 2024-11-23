#pragma once

#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Types.hpp"

namespace pregame
{
    struct NewWorldData
    {
        int Size;
    };

    struct MemberData
    {
        world::character::CharacterClasses Class;

        RaceTypes Race;
    };
}