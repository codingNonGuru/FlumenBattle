#include "Item.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Utility/Types.h"

using namespace world::character;

#define ITEM_COUNT_PER_GROUP 64

namespace world::character
{
    class ModifierAccessor
    {
    public:
        static void AddModifier(Character &character, Modifier modifier)
        {
            character.AddModifier(modifier);
        }
    };
}

class Sword : public ItemType
{
    using ItemType::ItemType; 

    void ApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_TYPE, (int)utility::RollDies::D6});
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_COUNT, 2});
    }
};

class Axe : public ItemType
{
    using ItemType::ItemType; 

    void ApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_TYPE, (int)utility::RollDies::D10});
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_COUNT, 1});
    }
};

class Spear : public ItemType
{
    using ItemType::ItemType; 

    void ApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_TYPE, (int)utility::RollDies::D8});
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_COUNT, 1});
    }
};

class Shield : public ItemType
{
    using ItemType::ItemType; 

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
    }*/
};

class Armor : public ItemType
{
    using ItemType::ItemType; 

    void ApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::BONUS_ARMOR_CLASS, 3});
        ModifierAccessor::AddModifier(character, {Modifiers::ARMOR_DEXTERITY_LIMIT, 2});
    }
};

class Helmet : public ItemType
{
    using ItemType::ItemType; 

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
    }*/
};

class Bow : public ItemType
{
    using ItemType::ItemType;

    bool IsRangedWeapon() const override {return true;} 

    void ApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_TYPE, (int)utility::RollDies::D8});
        ModifierAccessor::AddModifier(character, {Modifiers::BASE_ATTACK_DIE_COUNT, 1});
    }
};

class Food : public ItemType
{
    using ItemType::ItemType; 
};

bool Item::CanFitInto(ItemPositions position)
{
    switch(position)
    {
    case ItemPositions::MAIN_HAND:
        return Type->Use == ItemUses::WEAPON;
    case ItemPositions::OFF_HAND:
        return Type->Use == ItemUses::WEAPON || Type->Use == ItemUses::SHIELD;
    case ItemPositions::HEAD:
        return Type->Use == ItemUses::HEAD_WEAR;
    case ItemPositions::BODY:
        return Type->Use == ItemUses::BODY_WEAR;
    case ItemPositions::POUCH:
        return Type->Use == ItemUses::POTION;
    }
}

Word Item::GetTextureName(ItemTypes type)
{
    return ItemFactory::Get()->Create(type).Type->TextureName;
}

Item ItemFactory::Create(ItemTypes type)
{
    return {CreateType(type)};
}

const ItemType *ItemFactory::CreateType(ItemTypes type)
{
    switch(type)
    {
    case ItemTypes::SWORD:
        {
            static const auto itemType = Sword(type, "Sword", "SwordT1", ItemUses::WEAPON, 5);
            return &itemType;
        }
    case ItemTypes::ARMOR:
        {
            static const auto itemType = Armor(type, "Armor", "ArmorT1", ItemUses::BODY_WEAR, 8); 
            return &itemType;
        }
    case ItemTypes::SPEAR:
        {
            static const auto itemType = Spear(type, "Spear", "SpearT1", ItemUses::WEAPON, 3); 
            return &itemType;
        }
    case ItemTypes::SHIELD:
        {
            static const auto itemType = Shield(type, "Large shield", "ShieldLargeT1", ItemUses::SHIELD, 3); 
            return &itemType;
        }
    case ItemTypes::HELMET:
        {
            static const auto itemType = Helmet(type, "Helmet", "HelmetT1", ItemUses::HEAD_WEAR, 5); 
            return &itemType;
        }
    case ItemTypes::AXE:
        {
            static const auto itemType = Axe(type, "Axe", "AxeT1", ItemUses::WEAPON, 5); 
            return &itemType;
        }
    case ItemTypes::BOW:
        {
            static const auto itemType = Bow(type, "Bow", "BowT1", ItemUses::WEAPON, 5); 
            return &itemType;
        }
    case ItemTypes::FOOD:
        {
            static const auto itemType = Food(type, "Radish", "Radish", ItemUses::OTHER, 10); 
            return &itemType;
        }
    }
}

Item *ItemManager::Add(ItemTypes type, int amount)
{
    auto item = GetUnusedItem(type);
    if(item == nullptr)
    {
        auto newItem = ItemFactory::Get()->Create(type);
        newItem.Amount = amount;

        auto itemLocation = items.Add();
        *itemLocation = newItem;

        return itemLocation;
    }
    else
    {
        item->Amount += amount;
        
        return item;
    }
}

void ItemManager::Remove(Item *item)
{
    items.RemoveAt(item);
}

static auto itemsToBeRemoved = container::Array <Item *> (64);

void ItemManager::RemoveAmount(ItemTypes type, int amount)
{
    itemsToBeRemoved.Reset();

    auto amountLeftToRemove = amount;

    for(auto &item : items)
    {
        if(item.Type->Type == type)
        {
            if(amountLeftToRemove >= item.Amount)
            {
                amountLeftToRemove -= item.Amount;
                item.Amount = 0;

                *itemsToBeRemoved.Add() = &item;
            }
            else
            {
                item.Amount -= amountLeftToRemove;
                amountLeftToRemove = 0;
                break;
            }
        }
    }

    for(auto &item : itemsToBeRemoved)
    {
        items.RemoveAt(item);
    }
}

int ItemManager::GetAmount(ItemTypes type)
{
    auto amount = 0;

    for(auto item : items)
    {
        if(item.Type->Type == type)
        {
            amount += item.Amount;
        }
    }

    return amount;
}

Item *ItemManager::GetItem(ItemTypes type)
{
    return items.Find(type);
}

Item *ItemManager::GetUnusedItem(ItemTypes type)
{
    for(auto &item : items)
    {
        if(item.IsUsed == true)
            continue;

        if(item.Type->Type == type)
            return &item;
    }

    return nullptr;
}

Item *ItemManager::GetItem(int index)
{
    return items.Get(index);
}

int ItemManager::GetTotalWeight() const
{
    auto weight = 0;
    for(auto &item : items)
    {
        weight += item.Type->Weight * item.Amount;
    }

    return weight;
}