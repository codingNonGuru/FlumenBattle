#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Character/Types.h"

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

        bool isCharacterItem {false};

        world::character::ItemPositions itemPosition;

        void HandleConfigure() override;

        void SetItem(world::character::Item *newItem);

        void HandleLeftClick() override;

        void Select();

        void Deselect();
    };

    class InventoryMenu : public Element
    {
        friend class InventorySlot;

        Text *nameLabel;

        Text *classLabel;

        Element *border;

        character::Character *character;

        container::Array <InventorySlot *> inventorySlots;

        LayoutGroup *slotLayout;

        InventorySlot *selectedSlot {nullptr};

        InventorySlot *mainHandSlot; 
        
        InventorySlot *bodySlot; 
        
        InventorySlot *headSlot;

        Element *grabbedItem;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void SelectSlot(InventorySlot *slot);

        void GrabItem(InventorySlot *slot);

        void MoveItem(InventorySlot *slot);

        void SwapItem(InventorySlot *slot);

        void DropItem();

    public:
        void SelectCharacter(character::Character *);
    };
}