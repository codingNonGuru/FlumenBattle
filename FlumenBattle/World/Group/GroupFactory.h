#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

class Character;
struct BattleTile;

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
        static Group* Create(Pool <Group> &, GroupBuildData); 
    };
}