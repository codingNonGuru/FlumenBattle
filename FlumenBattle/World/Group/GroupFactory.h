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
    class GroupCore;
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
        static GroupCore* CreatePlayerGroup(GroupBuildData); 

        static GroupCore* CreateMachineGroup(GroupBuildData); 

        static void TransformIntoDeepGroup(GroupCore *);

        static void RevertToShallowGroup(GroupCore *);
    };
}