#pragma once

#include <functional>

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class GroupCore;

    class GroupAction
    {
        friend class GroupActionFactory;

        bool (*onCheck) (GroupCore &, const GroupActionData &);

        GroupActionResult (*onInitiate) (GroupCore &, const GroupActionData &) {nullptr};

        int (*onGetDuration) (const GroupCore &);

        GroupActionResult (*onPerform) (GroupCore &);

        GroupAction(
            GroupActions type, 
            Integer duration,
            bool hasVaryingIntensity, 
            bool (*_onCheck) (GroupCore &, const GroupActionData &), 
            GroupActionResult (*_onPerform) (GroupCore &),
            GroupActionResult (*_onInitiate) (GroupCore &, const GroupActionData &) = NULL,
            int (*_onGetDuration) (const GroupCore &) = nullptr) : 
            Type(type), BaseDuration(duration), HasVaryingIntensity(hasVaryingIntensity), onCheck(_onCheck), onInitiate(_onInitiate), onGetDuration(_onGetDuration), onPerform(_onPerform) {}

    public:
        static const int BASE_PROGRESS_RATE = 6;

        static const int BASE_ENCUMBRANCE_PENALTY = 2;

        GroupActions Type;

        Integer BaseDuration;

        bool HasVaryingIntensity;

        bool CanPerform(GroupCore &group, const GroupActionData &data = GroupActionData()) const;

        GroupActionResult Initiate(GroupCore &group, const GroupActionData &data = GroupActionData()) const
        {
            if(onInitiate != nullptr)
            {
                onInitiate(group, data);
            }

            return {Type};
        }

        int GetDuration(const GroupCore &group) const
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

        GroupActionResult Perform(GroupCore &group) const
        {
            return onPerform(group);
        }
    };
}