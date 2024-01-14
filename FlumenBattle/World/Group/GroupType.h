#pragma once

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/Types.h"

namespace world::group
{
    class GroupMind;

    struct GroupType
    {
        GroupTypes Type;

        GroupClasses Class;

        const GroupMind *Controller;

        int Size;
    };

    class GroupTypeFactory
    {
        static const GroupType * BuildPlayerGroupType();

        static const GroupType * BuildComputerGroupType(GroupClasses);

    public:
        static const GroupType * BuildGroupType(GroupClasses);
    };
}