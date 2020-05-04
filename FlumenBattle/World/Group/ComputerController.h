#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Group/GroupController.h"

namespace world::group
{
    class ComputerController : public GroupController, public Singleton <ComputerController>
    {
        void DetermineAction(Group &) const override;

        void PerformAction(Group &) const override;
    };
}