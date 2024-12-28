#include "GroupAction.h"
#include "GroupActionFactory.h"

using namespace world::group;

bool GroupAction::CanPerform(GroupCore &group, const GroupActionData &data) const
{
    return onCheck(group, data);
}