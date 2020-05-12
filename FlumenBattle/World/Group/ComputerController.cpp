#include "FlumenBattle/World/Group/ComputerController.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"

namespace world::group
{
    void ComputerController::DetermineAction(Group &group) const 
    {
        if(group.GetAction())  
            return;

        group.SelectAction(world::GroupActions::SEARCH);
    }

    void ComputerController::PerformAction(Group &) const 
    {

    }
}