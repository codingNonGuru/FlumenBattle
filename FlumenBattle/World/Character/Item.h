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
    enum class ItemTypes;

    struct ItemType
    {
        ItemTypes Type;

        Word TextureName;

        ItemUses Use {ItemUses::OTHER};

        ItemType(ItemTypes type, Word textureName) : Type(type), TextureName(textureName) {}

        ItemType(ItemTypes type, Word textureName, ItemUses use) : Type(type), TextureName(textureName), Use(use) {}
    };

    struct Item
    {
        const ItemType *Type;

        int Amount {1};

        bool IsUsed {false};

        bool CanFitInto(ItemPositions position);
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

        void Add(ItemTypes);

        void Remove(Item *);

        int GetAmount(ItemTypes);

        Item *GetItem(int);
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