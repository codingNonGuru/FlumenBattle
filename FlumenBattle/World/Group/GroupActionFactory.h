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

        const GroupAction * BuildFight();

        const GroupAction * BuildTravel();

    public:
        const GroupAction * BuildAction(GroupActions);
    };

    class GroupActionPerformer
    {
        friend class GroupActionFactory;

        static void InitiateTravel(Group &, const GroupActionData &);

        static int GetTravelDuration(const Group &);

        static void TakeShortRest(Group &group);

        static void TakeLongRest(Group &group);

        static void Search(Group &group);

        static void Fight(Group &group);

        static void Travel(Group &group);
    };

    class GroupActionValidator
    {
        friend class GroupActionFactory;

        static bool CanTakeShortRest(Group &, const GroupActionData &);

        static bool CanTakeLongRest(Group &, const GroupActionData &);

        static bool CanSearch(Group &, const GroupActionData &);

        static bool CanFight(Group &, const GroupActionData &);

        static bool CanTravel(Group &, const GroupActionData &);
    };
}