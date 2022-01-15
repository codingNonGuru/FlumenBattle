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

        void (*onInitiate) (Group &, const GroupActionData &);

        void (*onPerform) (Group &);

        GroupAction(
            GroupActions _type, 
            Integer _duration, 
            bool (*_onCheck) (Group &, const GroupActionData &), 
            void (*_onPerform) (Group &),
            void (*_onInitiate) (Group &, const GroupActionData &) = nullptr) : 
            Type(_type), Duration(_duration), onCheck(_onCheck), onInitiate(_onInitiate), onPerform(_onPerform) {}

    public:
        GroupActions Type;

        Integer Duration;

        bool CanPerform(Group &group, const GroupActionData &data = GroupActionData());

        void Initiate(Group &group, const GroupActionData &data = GroupActionData())
        {
            if(onInitiate)
            {
                onInitiate(group, data);
            }
        }

        void Perform(Group &group) const
        {
            onPerform(group);
        }
    };
}