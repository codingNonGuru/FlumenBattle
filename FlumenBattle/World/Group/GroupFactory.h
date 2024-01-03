#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

namespace world::character
{
    class Character;
}

namespace battle
{
    struct BattleTile;
}

namespace world::group
{
    class Group;

    struct GroupBuildData
    {
        GroupTypes Type;

        RaceTypes Race;
    };

    class GroupFactory
    {
    public:
        static Group* Create(GroupBuildData); 
    };
}