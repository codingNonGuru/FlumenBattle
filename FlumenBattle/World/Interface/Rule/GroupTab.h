#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Group/Types.h"

class Text;
class SimpleList;

namespace world::interface
{
    class ResourceCounter;
}

namespace world::group
{
    class GroupCore;
}

namespace world::interface::rule
{
    class GroupItem : public Element
    {
        Text *leaderNameLabel;

        ResourceCounter *sizeCounter;

        int groupSize;

        ResourceCounter *levelCounter;

        int groupLevel;

        void HandleConfigure() override;

    public:
        void Setup(const group::GroupCore *);
    };

    class GroupBatchItem : public Element
    {
        Text *mainLabel;

        group::GroupClasses groupClass;

        container::Array <GroupItem *> items;

        SimpleList *itemList;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(group::GroupClasses);
    };

    class GroupTab : public Element
    {
        container::Array <GroupBatchItem *> batchItems;

        SimpleList *itemList;

        void HandleConfigure() override;
    };
}