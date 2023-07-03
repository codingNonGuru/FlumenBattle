#pragma once

#include "FlumenBattle/World/Types.h"

namespace world::group
{
    class GroupMind;

    struct GroupType
    {
        GroupTypes Type;

        const GroupMind *Controller;

        int Size;
    };

    class GroupTypeFactory
    {
        static const GroupType * BuildPlayerGroupType();

        static const GroupType * BuildComputerGroupType();

    public:
        static const GroupType * BuildGroupType(GroupTypes);
    };
}