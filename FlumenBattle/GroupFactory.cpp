#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"

Color colors[] = {Color::RED, Color::GREEN};
Integer colorIndex = 0;

Color GetColor()
{
    auto color = colors[colorIndex];

    colorIndex++;
    if(colorIndex == 2)
        colorIndex = 0;

    return color;
}

Group* GroupFactory::Create(Array <Group> &groups, GroupBuildData buildData)
{
    auto group = groups.Allocate();

    auto size = 7; //utility::GetRandom(4, 7);
    auto color = GetColor();
    group->Initialize(size, color, buildData.Race);

    return group;
}