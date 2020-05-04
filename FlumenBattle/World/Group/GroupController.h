#pragma once

#include "FlumenBattle/Types.hpp"

namespace world::group
{
    class Group;

    class GroupController
    {
        friend class Group;

        virtual void DetermineAction(Group &) const {}

        virtual void PerformAction(Group &) const {}
    };  
};