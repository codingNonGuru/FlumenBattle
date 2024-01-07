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
    class InventoryMenu;

    class InventorySlot : public Element
    {
        friend class InventoryMenu;

        Element *icon;

        Element *border;

        world::character::Item *item {nullptr};

        bool isSelected {false};

        InventoryMenu *menu;

        void HandleConfigure() override;

        void SetItem(world::character::Item *newItem);

        void HandleLeftClick() override;

        void Select();

        void Deselect();
    };

    class InventoryMenu : public Element
    {
        friend class InventorySlot;

        Text *characterLabel;

        Element *border;

        character::Character *character;

        container::Array <InventorySlot *> inventorySlots;

        LayoutGroup *slotLayout;

        InventorySlot *selectedSlot {nullptr};

        Element *grabbedItem;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void SelectSlot(InventorySlot *slot);

    public:
        void SelectCharacter(character::Character *);
    };
}