#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class ResourceCounter;
    class InventorySlot;
}

namespace world::character
{
    struct Item;
}

namespace world::interface
{
    class ItemHoverInfo : public Element
    {
        Element *border;

        LayoutGroup *layout;

        Text *nameLabel;

        Text *classLabel;

        LayoutGroup *counterGroup;

        world::interface::ResourceCounter *valueCounter;

        world::interface::ResourceCounter *weightCounter;

        InventorySlot *hoveredSlot {nullptr};

        character::Item *item;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(InventorySlot *);
    };
}