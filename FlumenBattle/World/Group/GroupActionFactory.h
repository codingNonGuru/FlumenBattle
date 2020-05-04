#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Types.h"

namespace world::group
{
    class Group;
    class GroupAction;

    class GroupActionFactory : public Singleton<GroupActionFactory>
    {
        const GroupAction * BuildTakeShortRest();

        const GroupAction * BuildTakeLongRest();

        const GroupAction * BuildSearch();

    public:
        const GroupAction * BuildAction(GroupActions);
    };

    class GroupActionPerformer
    {
        friend class GroupActionFactory;

        static void TakeShortRest(Group &group);

        static void TakeLongRest(Group &group);

        static void Search(Group &group);
    };

    class GroupActionValidator
    {
        friend class GroupActionFactory;

        static bool CanTakeShortRest(Group &group);

        static bool CanTakeLongRest(Group &group);

        static bool CanSearch(Group &group);
    };
}