#pragma once

namespace world::group
{
    class GroupCore;

    struct GroupEssence
    {
        GroupCore *Group {nullptr};

        bool operator== (const group::GroupCore *group) {return Group == group;}
    };
}