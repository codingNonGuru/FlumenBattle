#pragma once

#include "FlumenBattle/World/Types.h"

namespace world::group
{
    class GroupController;

    struct GroupType
    {
        GroupTypes Type;

        const GroupController *Controller;

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