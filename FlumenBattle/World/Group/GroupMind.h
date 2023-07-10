#pragma once

#include "FlumenBattle/Types.hpp"

#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class Group;
    struct GroupActionResult;

    class GroupMind
    {
        friend class Group;

        virtual void DetermineAction(Group &) const {}

        virtual void RegisterActionPerformance(Group &, GroupActionResult) const {}

        virtual void RegisterActionInitiation(Group &, GroupActionResult) const {}
    };  
};