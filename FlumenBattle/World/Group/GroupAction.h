#pragma once

#include <functional>

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class Group;

    class GroupAction
    {
        friend class GroupActionFactory;

        bool (*onCheck) (Group &, const GroupActionData &);

        GroupActionResult (*onInitiate) (Group &, const GroupActionData &) {nullptr};

        int (*onGetDuration) (const Group &);

        GroupActionResult (*onPerform) (Group &);

        GroupAction(
            GroupActions type, 
            Integer duration,
            bool hasVaryingIntensity, 
            bool (*_onCheck) (Group &, const GroupActionData &), 
            GroupActionResult (*_onPerform) (Group &),
            GroupActionResult (*_onInitiate) (Group &, const GroupActionData &) = NULL,
            int (*_onGetDuration) (const Group &) = nullptr) : 
            Type(type), BaseDuration(duration), HasVaryingIntensity(hasVaryingIntensity), onCheck(_onCheck), onInitiate(_onInitiate), onGetDuration(_onGetDuration), onPerform(_onPerform) {}

    public:
        static const int BASE_PROGRESS_RATE = 6;

        static const int BASE_ENCUMBRANCE_PENALTY = 2;

        GroupActions Type;

        Integer BaseDuration;

        bool HasVaryingIntensity;

        bool CanPerform(Group &group, const GroupActionData &data = GroupActionData()) const;

        GroupActionResult Initiate(Group &group, const GroupActionData &data = GroupActionData()) const
        {
            if(onInitiate != nullptr)
            {
                onInitiate(group, data);
            }

            return {Type};
        }

        int GetDuration(const Group &group) const
        {
            if(onGetDuration != nullptr)
            {
                return onGetDuration(group);
            }
            else
            {
                return BaseDuration;
            }
        }

        GroupActionResult Perform(Group &group) const
        {
            return onPerform(group);
        }
    };
}