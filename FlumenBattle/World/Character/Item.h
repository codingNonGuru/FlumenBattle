#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Character/Types.h"

namespace world::group
{
    class GroupAllocator;
    class Group;
}

namespace world::character
{
    class Character;
    enum class ItemTypes;

    struct ItemType
    {
        ItemTypes Type;

        Word TextureName;

        int Weight {1};

        ItemUses Use {ItemUses::OTHER};

        ItemType(ItemTypes type, Word textureName) : Type(type), TextureName(textureName) {}

        ItemType(ItemTypes type, Word textureName, ItemUses use) : Type(type), TextureName(textureName), Use(use) {}

        virtual void ApplyEffect(Character &) const {}

        virtual bool IsRangedWeapon() const {return false;}
    };

    struct Item
    {
        const ItemType *Type;

        int Amount {1};

        bool IsUsed {false};

        bool CanFitInto(ItemPositions position);

        void ApplyEffect(Character &character) const {Type->ApplyEffect(character);}

        bool IsRangedWeapon() const {return Type->IsRangedWeapon();}

        bool operator==(ItemTypes type) {return Type->Type == type;}
    };

    class ItemSet
    {
    protected:
        container::Pool <Item> items;

        ItemSet() {}
    };

    class ItemManager : protected ItemSet
    {
        friend class ItemAllocator;

        friend class group::Group;

        Item *Add(ItemTypes, int = 1);

        void Remove(Item *);

        void RemoveAmount(ItemTypes, int);

        int GetAmount(ItemTypes);

        Item *GetItem(ItemTypes);

        Item *GetItem(int);

        Item *GetUnusedItem(ItemTypes);

        int GetTotalWeight();

        const container::Pool <Item> &GetItems() const {return items;}
    };

    class ItemFactory : public core::Singleton <ItemFactory>
    {
        friend class ItemManager;

        Item Create(ItemTypes);
    };

    class ItemAllocator
    {
        friend class world::group::GroupAllocator;

        static void Allocate(container::PoolAllocator <Item> &allocator, ItemManager &manager) 
        {
            manager.items.Initialize(allocator);
        }

        static void Free(container::PoolAllocator <Item> &allocator, ItemManager &manager) 
        {
            manager.items.Terminate(allocator);
        }
    };
}