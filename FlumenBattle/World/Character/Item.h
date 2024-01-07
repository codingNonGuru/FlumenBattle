#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

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

        ItemType(ItemTypes type, Word textureName) : Type(type), TextureName(textureName) {}
    };

    struct Item
    {
        const ItemType *Type;

        int Amount;

        bool IsUsed;
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