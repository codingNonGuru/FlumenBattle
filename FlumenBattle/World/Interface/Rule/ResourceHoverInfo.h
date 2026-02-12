#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class SimpleList;

namespace world::settlement
{
    struct Resource;
}

namespace world::interface::rule
{
    class ResourceItem;
    class RaceItem;

    class ResourceHoverInfo : public Element
    {
        Element *border;

        ResourceItem *hoveredItem;

        SimpleList *raceItemGroup;

        container::Array <RaceItem *> raceItems;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(ResourceItem *);
    };
}