#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world::group
{
    class GroupCore;
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

        const GroupAction * BuildPillageSettlement();

        const GroupAction * BuildBribeGarrison();

    public:
        const GroupAction * BuildAction(GroupActions);
    };

    class GroupActionPerformer
    {
        friend class GroupActionFactory;

        static GroupActionResult InitiateTravel(GroupCore &, const GroupActionData &);

        static GroupActionResult InitiateEngage(GroupCore &, const GroupActionData &);

        static int GetTravelDuration(const GroupCore &);

        static GroupActionResult TakeShortRest(GroupCore &group);

        static GroupActionResult TakeLongRest(GroupCore &group);

        static GroupActionResult Search(GroupCore &group);

        static GroupActionResult Engage(GroupCore &group);

        static GroupActionResult Fight(GroupCore &group);

        static GroupActionResult Disengage(GroupCore &group);

        static GroupActionResult Travel(GroupCore &group);

        static GroupActionResult Persuade(GroupCore &group);

        static GroupActionResult Forage(GroupCore &group);

        static GroupActionResult BypassDefences(GroupCore &group);

        static GroupActionResult LootSettlement(GroupCore &group);

        static GroupActionResult PillageSettlement(GroupCore &group);

        static GroupActionResult BribeGarrison(GroupCore &group);
    };

    class GroupActionValidator
    {
        friend class GroupActionFactory;

        static bool CanTakeShortRest(GroupCore &, const GroupActionData &);

        static bool CanTakeLongRest(GroupCore &, const GroupActionData &);

        static bool CanSearch(GroupCore &, const GroupActionData &);

        static bool CanEngage(GroupCore &, const GroupActionData &);

        static bool CanFight(GroupCore &, const GroupActionData &);

        static bool CanBypassDefences(GroupCore &, const GroupActionData &);

        static bool CanDisengage(GroupCore &, const GroupActionData &);

        static bool CanTravel(GroupCore &, const GroupActionData &);

        static bool CanPersuade(GroupCore &, const GroupActionData &);

        static bool CanForage(GroupCore &, const GroupActionData &);

        static bool CanLootSettlement(GroupCore &, const GroupActionData &);

        static bool CanPillageSettlement(GroupCore &, const GroupActionData &);

        static bool CanBribeGarrison(GroupCore &, const GroupActionData &);
    };
}