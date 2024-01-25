#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Group/Types.h"

namespace world::character
{
    class Character;
}

namespace world::group
{
    class Group;

    struct GroupSpotting
    {
        Group *Group;

        int GroupUniqueId;

        GroupClasses GroupClass;

        Word LeaderName;

        int TimeInHours;

        bool IsQuality;

        int Distance;

        bool operator== (const int otherGroupId) {return otherGroupId == GroupUniqueId;}
    };
}