#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::character
{
    class Character;
    class Item;
}

namespace world::interface
{
    class InventorySlot : public Element
    {
        friend class InventoryMenu;

        Element *icon;

        world::character::Item *item {nullptr};

        void HandleConfigure() override;

        void SetItem(world::character::Item *newItem);
    };

    class InventoryMenu : public Element
    {
        Text *characterLabel;

        Element *border;

        character::Character *character;

        container::Array <InventorySlot *> inventorySlots;

        LayoutGroup *slotLayout;

        void HandleConfigure() override;

    public:
        void SelectCharacter(character::Character *);
    };
}