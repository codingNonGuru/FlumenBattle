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

namespace world::settlement
{
    class Settlement;
}

namespace world::group
{
    class Group;

    struct GroupBuildData
    {
        GroupTypes Type;

        RaceTypes Race;

        settlement::Settlement *Home;
    };

    class GroupFactory
    {
    public:
        static Group* Create(GroupBuildData); 
    };
}