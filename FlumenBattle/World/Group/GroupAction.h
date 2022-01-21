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

        GroupActionResult (*onInitiate) (Group &, const GroupActionData &);

        int (*onGetDuration) (const Group &);

        void (*onPerform) (Group &);

        GroupAction(
            GroupActions type, 
            Integer duration, 
            bool (*_onCheck) (Group &, const GroupActionData &), 
            void (*_onPerform) (Group &),
            GroupActionResult (*_onInitiate) (Group &, const GroupActionData &) = nullptr,
            int (*_onGetDuration) (const Group &) = nullptr) : 
            Type(type), BaseDuration(duration), onCheck(_onCheck), onInitiate(_onInitiate), onGetDuration(_onGetDuration), onPerform(_onPerform) {}

    public:
        GroupActions Type;

        Integer BaseDuration;

        bool CanPerform(Group &group, const GroupActionData &data = GroupActionData());

        GroupActionResult Initiate(Group &group, const GroupActionData &data = GroupActionData())
        {
            if(onInitiate)
            {
                return onInitiate(group, data);
            }
        }

        int GetDuration(const Group &group) const
        {
            if(onGetDuration)
            {
                return onGetDuration(group);
            }
        }

        void Perform(Group &group) const
        {
            onPerform(group);
        }
    };
}