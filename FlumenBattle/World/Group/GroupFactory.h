#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

namespace pregame
{
    struct MemberData;
}

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
    enum class GroupClasses;

    struct GroupBuildData
    {
        GroupClasses Type;

        RaceTypes Race;

        settlement::Settlement *Home;

        const container::Array <pregame::MemberData> *MemberDatas {nullptr};
    };

    class GroupFactory
    {
    public:
        static Group* CreatePlayerGroup(GroupBuildData); 

        static Group* CreateMachineGroup(GroupBuildData); 
    };
}