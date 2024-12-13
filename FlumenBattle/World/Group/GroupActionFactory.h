#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class Group;
    class GroupAction;

    class GroupActionFactory : public core::Singleton<GroupActionFactory>
    {
        const GroupAction * BuildTakeShortRest();

        const GroupAction * BuildTakeLongRest();

        const GroupAction * BuildSearch();

        const GroupAction * BuildEngage();

        const GroupAction * BuildFight();

        const GroupAction * BuildDisengage();

        const GroupAction * BuildTravel();

        const GroupAction * BuildPersuade();

        const GroupAction * BuildForage();

        const GroupAction * BuildBypassDefences();

        const GroupAction * BuildLootSettlement();

    public:
        const GroupAction * BuildAction(GroupActions);
    };

    class GroupActionPerformer
    {
        friend class GroupActionFactory;

        static GroupActionResult InitiateTravel(Group &, const GroupActionData &);

        static GroupActionResult InitiateEngage(Group &, const GroupActionData &);

        static int GetTravelDuration(const Group &);

        static GroupActionResult TakeShortRest(Group &group);

        static GroupActionResult TakeLongRest(Group &group);

        static GroupActionResult Search(Group &group);

        static GroupActionResult Engage(Group &group);

        static GroupActionResult Fight(Group &group);

        static GroupActionResult Disengage(Group &group);

        static GroupActionResult Travel(Group &group);

        static GroupActionResult Persuade(Group &group);

        static GroupActionResult Forage(Group &group);

        static GroupActionResult BypassDefences(Group &group);

        static GroupActionResult LootSettlement(Group &group);
    };

    class GroupActionValidator
    {
        friend class GroupActionFactory;

        static bool CanTakeShortRest(Group &, const GroupActionData &);

        static bool CanTakeLongRest(Group &, const GroupActionData &);

        static bool CanSearch(Group &, const GroupActionData &);

        static bool CanEngage(Group &, const GroupActionData &);

        static bool CanFight(Group &, const GroupActionData &);

        static bool CanBypassDefences(Group &, const GroupActionData &);

        static bool CanDisengage(Group &, const GroupActionData &);

        static bool CanTravel(Group &, const GroupActionData &);

        static bool CanPersuade(Group &, const GroupActionData &);

        static bool CanForage(Group &, const GroupActionData &);

        static bool CanLootSettlement(Group &, const GroupActionData &);
    };
}