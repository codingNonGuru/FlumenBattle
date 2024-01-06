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
        world::character::Item *item;
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