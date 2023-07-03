#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class Group;
    class GroupAction;

    class GroupActionFactory : public Singleton<GroupActionFactory>
    {
        const GroupAction * BuildTakeShortRest();

        const GroupAction * BuildTakeLongRest();

        const GroupAction * BuildSearch();

        const GroupAction * BuildEngage();

        const GroupAction * BuildFight();

        const GroupAction * BuildTravel();

    public:
        const GroupAction * BuildAction(GroupActions);
    };

    class GroupActionPerformer
    {
        friend class GroupActionFactory;

        static GroupActionResult InitiateTravel(Group &, const GroupActionData &);

        static int GetTravelDuration(const Group &);

        static GroupActionResult TakeShortRest(Group &group);

        static GroupActionResult TakeLongRest(Group &group);

        static GroupActionResult Search(Group &group);

        static GroupActionResult Engage(Group &group);

        static GroupActionResult Fight(Group &group);

        static GroupActionResult Travel(Group &group);
    };

    class GroupActionValidator
    {
        friend class GroupActionFactory;

        static bool CanTakeShortRest(Group &, const GroupActionData &);

        static bool CanTakeLongRest(Group &, const GroupActionData &);

        static bool CanSearch(Group &, const GroupActionData &);

        static bool CanEngage(Group &, const GroupActionData &);

        static bool CanFight(Group &, const GroupActionData &);

        static bool CanTravel(Group &, const GroupActionData &);
    };
}