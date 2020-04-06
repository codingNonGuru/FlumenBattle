#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"

Group* GroupFactory::Create(Array <Group> &groups)
{
    auto group = groups.Allocate();

    group->Initialize();

    return group;
}