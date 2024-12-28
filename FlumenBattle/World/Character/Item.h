#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Character/Types.h"

namespace world::group
{
    class GroupAllocator;
    class GroupCore;
    class GroupExtraData;
}

namespace world::character
{
    class Character;
    enum class ItemTypes;

    struct ItemType
    {
        ItemTypes Type;

        Word Name;

        Word TextureName;

        int Weight {1};

        ItemUses Use {ItemUses::OTHER};

        int Value {1};

        bool IsResource {false};

        ItemType(ItemTypes type, Word name, Word textureName) : Type(type), Name(name), TextureName(textureName) {}

        ItemType(ItemTypes type, Word name, Word textureName, ItemUses use) : Type(type), Name(name), TextureName(textureName), Use(use) {}

        ItemType(ItemTypes type, Word name, Word textureName, int value) : Type(type), Name(name), TextureName(textureName), Value(value) {}

        ItemType(ItemTypes type, Word name, Word textureName, ItemUses use, int value) : Type(type), Name(name), TextureName(textureName), Use(use), Value(value) {}

        virtual void ApplyEffect(Character &) const {}

        virtual bool IsRangedWeapon() const {return false;}
    };

    struct Item
    {
        const ItemType *Type;

        int Amount {1};

        bool IsUsed {false};

        bool CanFitInto(ItemPositions);

        void ApplyEffect(Character &character) const {Type->ApplyEffect(character);}

        bool IsRangedWeapon() const {return Type->IsRangedWeapon();}

        bool operator==(ItemTypes type) {return Type->Type == type;}

        static Word GetTextureName(ItemTypes);
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

        friend class group::GroupExtraData;

        Item *Add(ItemTypes, int = 1);

        void Remove(Item *);

        void RemoveAmount(ItemTypes, int);

        int GetAmount(ItemTypes);

        Item *GetItem(ItemTypes);

        Item *GetItem(int);

        Item *GetUnusedItem(ItemTypes);

        int GetTotalWeight() const;

        const container::Pool <Item> &GetItems() const {return items;}
    };

    class ItemFactory : public core::Singleton <ItemFactory>
    {
        friend class Item;

        friend class ItemManager;

    public:
        Item Create(ItemTypes);

        const ItemType *CreateType(ItemTypes);
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