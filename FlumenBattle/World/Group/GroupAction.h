#pragma once

#include <functional>

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Types.h"

namespace world::group
{
    class Group;

    class GroupAction
    {
        friend class GroupActionFactory;

        bool (*onCheck) (Group &);

        void (*onPerform) (Group &);

        GroupAction(GroupActions _type, Integer _duration, bool (*_onCheck) (Group &), void (*_onPerform) (Group &)) : 
            Type(_type), Duration(_duration), onCheck(_onCheck), onPerform(_onPerform) {}

    public:
        GroupActions Type;

        Integer Duration;

        bool CanPerform(Group &group) const
        {
            return onCheck(group);
        }

        void Perform(Group &group) const
        {
            onPerform(group);
        }
    };
}