#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

class Delegate;

namespace world
{
    class WorldTile;
}

namespace utility
{
    class WorldPathData;
}

namespace world::group
{
    struct GroupActionResult;

    class HumanMind : public GroupMind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        HumanMind();

        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;

        void RegisterActionInitiation(Group &, GroupActionResult) const override;

        void HandleSearch();

        void HandleTakeQuickRest();

        void HandleTakeLongRest();

        void HandleTravel();

        void HandleResumeTravel();

        void HandleCancelTravel();

        void HandleSlackenAction();

        void HandleIntensifyAction();

    public:
        Delegate *OnActionSelected;

        Delegate *OnActionPerformed;

        Delegate *OnActionInitiated;

        Delegate *OnSkillCheckRolled;

        void EnableInput();

        void DisableInput();

        const GroupActionResult & GetSelectedActionResult();

        const GroupActionResult & GetPerformedActionResult();

        const utility::WorldPathData GetFullPathData();

        world::WorldTile *GetFinalDestination() const;
    };  
};