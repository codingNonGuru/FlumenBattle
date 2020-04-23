#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"

Array <Color> colors = {Color::RED, Color::GREEN, Color::CYAN, Color::RED * 0.5f, Color::GREEN * 0.5f, Color::GREEN, Color::BLUE};

Array <Word> names = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};

int nameIndex = 0;

Color GetColor()
{
    static Integer colorIndex = 0;
    auto color = colors[colorIndex];

    colorIndex++;
    if(colorIndex == colors.GetSize())
        colorIndex = 0;

    return color;
}

Group* GroupFactory::Create(Array <Group> &groups, GroupBuildData buildData)
{
    auto group = groups.Allocate();

    auto size = 1; //utility::GetRandom(4, 7);
    auto color = GetColor();
    group->Initialize(*names.Get(nameIndex), size, color, buildData.Race);

    nameIndex++;

    return group;
}