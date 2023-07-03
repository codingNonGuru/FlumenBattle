#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"

namespace world::group
{
    void MachineMind::DetermineAction(Group &group) const 
    {
        if(group.GetAction())  
            return;

        group.SelectAction(GroupActions::SEARCH);
    }

    void MachineMind::RegisterActionPerformance(Group &, GroupActionResult) const
    {

    }
}