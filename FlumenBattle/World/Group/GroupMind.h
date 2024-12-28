#pragma once

#include "FlumenBattle/Types.hpp"

#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class GroupCore;
    struct GroupActionResult;

    class GroupMind
    {
        friend class GroupCore;

        friend class GroupExtraData;

        virtual void DetermineAction(GroupCore &) const {}

        virtual void RegisterActionPerformance(GroupCore &, GroupActionResult) const {}

        virtual void RegisterActionInitiation(GroupCore &, GroupActionResult) const {}
    };  
};