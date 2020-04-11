#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"

Color colors[] = {Color::RED, Color::GREEN};
Integer colorIndex = 0;

Group* GroupFactory::Create(Array <Group> &groups)
{
    auto group = groups.Allocate();

    *group = Group();

    auto size = 5;//utility::GetRandom(4, 7);
    auto color = colors[colorIndex++];
    group->Initialize(size, color);

    return group;
}