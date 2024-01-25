#pragma once

namespace world::group
{
    class Group;

    struct GroupSpotting
    {
        Group *Group;

        int TimeInHours;

        bool IsQuality;

        int Distance;

        bool operator== (const class Group *group) {return group == Group;}
    };
}