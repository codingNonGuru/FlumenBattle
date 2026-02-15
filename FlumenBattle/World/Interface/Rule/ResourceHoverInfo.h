#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class SimpleList;
class Text;

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

        Text *raceLabel;

        ResourceItem *hoveredItem;

        SimpleList *raceItemGroup;

        container::Array <RaceItem *> raceItems;

        bool isTileBased;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleRaceShifted();

        void RefreshItems();

    public:
        void Setup(ResourceItem *);

        void Setup();
    };
}