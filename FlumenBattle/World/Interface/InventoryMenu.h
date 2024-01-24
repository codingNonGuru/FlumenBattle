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
    class Counter;
    class ResourceCounter;

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

        Counter *counter;

        void HandleConfigure() override;

        void SetItem(world::character::Item *newItem);

        void HandleLeftClick() override;

        void HandleHover() override;

        void Select();

        void Deselect();

    public:
        character::Item *GetItem() {return item;}
    };

    class InventoryMenu : public Element
    {
        friend class InventorySlot;

        Text *nameLabel;

        Text *classLabel;

        Text *healthLabel;

        Text *armorLabel;

        Text *attackLabel;

        Text *damageLabel;

        ResourceCounter *weightCounter;

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

        void HandleEnable() override;

        void SelectSlot(InventorySlot *);

        void GrabItem(InventorySlot *);

        void MoveItem(InventorySlot *);

        void SwapItem(InventorySlot *);

        void DropItem();

        void SellItem(InventorySlot *);

        void HandleItemAdded();

    public:
        void SelectCharacter(character::Character *);
    };
}