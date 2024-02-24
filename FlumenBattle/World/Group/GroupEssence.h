#pragma once

namespace world::group
{
    class Group;

    struct GroupEssence
    {
        Group *Group {nullptr};

        bool operator== (const group::Group *group) {return Group == group;}
    };
}