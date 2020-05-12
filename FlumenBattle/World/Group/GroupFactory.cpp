#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"

Array <Color> colors = {Color::RED, Color::GREEN, Color::CYAN, Color::RED * 0.5f, Color::GREEN * 0.5f, Color::GREEN, Color::BLUE};

Array <Word> names = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};

int nameIndex = 0;

namespace world::group
{
    Color GetColor()
    {
        static Integer colorIndex = 0;
        auto color = colors[colorIndex];

        colorIndex++;
        if(colorIndex == colors.GetSize())
            colorIndex = 0;

        return color;
    }

    Group* GroupFactory::Create(Pool <Group> &groups, GroupBuildData buildData)
    {
        auto group = groups.Add();

        auto type = GroupTypeFactory::BuildGroupType(buildData.Type);

        auto size = type->Size; //utility::GetRandom(4, 7);

        auto color = GetColor();

        group->Initialize(*names.Get(nameIndex), type, size, color, buildData.Race);

        nameIndex++;

        return group;
    }
}