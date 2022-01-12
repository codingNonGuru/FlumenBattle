#pragma once

#include <functional>

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class Group;

    class GroupAction
    {
        friend class GroupActionFactory;

        bool (*onCheck) (Group &, const GroupActionData &);

        void (*onPerform) (Group &);

        GroupAction(GroupActions _type, Integer _duration, bool (*_onCheck) (Group &, const GroupActionData &), void (*_onPerform) (Group &)) : 
            Type(_type), Duration(_duration), onCheck(_onCheck), onPerform(_onPerform) {}

    public:
        GroupActions Type;

        Integer Duration;

        bool CanPerform(Group &group, const GroupActionData &data = GroupActionData());

        void Perform(Group &group) const
        {
            onPerform(group);
        }
    };
}