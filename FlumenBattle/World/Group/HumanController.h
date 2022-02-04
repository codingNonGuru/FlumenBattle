#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Group/GroupController.h"

class Delegate;

namespace world::group
{
    struct GroupActionResult;

    class HumanController : public GroupController, public Singleton <HumanController>
    {
        void DetermineAction(Group &) const override;

        void PerformAction(Group &) const override;

        void HandleSearch();

        void HandleTakeShortRest();

        void HandleTravel();

        void HandleSlackenAction();

        void HandleIntensifyAction();

    public:
        Delegate *OnActionSelected;

        Delegate *OnActionPerformed;

        HumanController();

        void EnableInput();

        void DisableInput();

        const GroupActionResult & GetSelectedActionResult();
    };  
};