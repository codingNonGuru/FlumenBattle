#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

class Delegate;

namespace world::group
{
    struct GroupActionResult;

    class HumanMind : public GroupMind, public Singleton <HumanMind>
    {
        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;

        void RegisterActionInitiation(Group &, GroupActionResult) const override;

        void HandleSearch();

        void HandleTakeShortRest();

        void HandleTakeLongRest();

        void HandleTravel();

        void HandleSlackenAction();

        void HandleIntensifyAction();

    public:
        Delegate *OnActionSelected;

        Delegate *OnActionPerformed;

        Delegate *OnActionInitiated;

        Delegate *OnSkillCheckRolled;

        HumanMind();

        void EnableInput();

        void DisableInput();

        const GroupActionResult & GetSelectedActionResult();

        const GroupActionResult & GetPerformedActionResult();
    };  
};