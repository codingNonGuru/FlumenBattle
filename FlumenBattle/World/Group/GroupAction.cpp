#include "GroupAction.h"
#include "GroupActionFactory.h"

using namespace world::group;

bool GroupAction::CanPerform(Group &group, const GroupActionData &data)
{
    return onCheck(group, data);
}