#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"

Color colors[] = {Color::RED, Color::GREEN};
Integer colorIndex = 0;

Group* GroupFactory::Create(Array <Group> &groups, GroupBuildData buildData)
{
    auto group = groups.Allocate();

    group->tile = buildData.Tile;

    auto size = 8;//utility::GetRandom(4, 7);
    auto color = colors[colorIndex++];
    group->Initialize(size, color);

    return group;
}