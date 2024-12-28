#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Group/Types.h"

namespace world::character
{
    class Character;
    enum class CharacterClasses;
}

namespace world::group
{
    class GroupCore;

    struct GroupSpotting
    {
        GroupCore *Group;

        int GroupUniqueId;

        GroupClasses GroupClass;

        Word LeaderName;

        Word OriginName;

        int TimeInHours;

        bool IsQuality;

        int Distance;

        Position2 VisualPosition;

        bool IsFacingRightwards;

        container::Array <character::CharacterClasses> Characters;

        bool operator== (const int otherGroupId) {return otherGroupId == GroupUniqueId;}
    };
}