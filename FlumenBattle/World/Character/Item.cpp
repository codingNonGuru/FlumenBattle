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

Item ItemFactory::Create(ItemTypes type)
{
    switch(type)
    {
    case ItemTypes::SWORD:
        return 
        {
            [&] {static const auto itemType = Sword(type, "SwordT1", ItemUses::WEAPON); return &itemType;} ()
        };
    case ItemTypes::ARMOR:
        return 
        {
            [&] {static const auto itemType = Armor(type, "ArmorT1", ItemUses::BODY_WEAR); return &itemType;} ()
        };
    case ItemTypes::SPEAR:
        return 
        {
            [&] {static const auto itemType = Spear(type, "SpearT1", ItemUses::WEAPON); return &itemType;} ()
        };
    case ItemTypes::SHIELD:
        return 
        {
            [&] {static const auto itemType = Shield(type, "ShieldLargeT1", ItemUses::SHIELD); return &itemType;} ()
        };
    case ItemTypes::HELMET:
        return {[&] {static const auto itemType = Helmet(type, "HelmetT1", ItemUses::HEAD_WEAR); return &itemType;} ()};
    case ItemTypes::AXE:
        return {[&] {static const auto itemType = Axe(type, "AxeT1", ItemUses::WEAPON); return &itemType;} ()};
    case ItemTypes::BOW:
        return {[&] {static const auto itemType = Bow(type, "BowT1", ItemUses::WEAPON); return &itemType;} ()};
    case ItemTypes::FOOD:
        return {[&] {static const auto itemType = Food(type, "Carrot", ItemUses::OTHER); return &itemType;} ()};
    }
}

void ItemManager::Add(ItemTypes type, int amount)
{
    auto item = GetItem(type);
    if(item == nullptr)
    {
        auto newItem = ItemFactory::Get()->Create(type);
        newItem.Amount = amount;
        *items.Add() = newItem;
    }
    else
    {
        item->Amount += amount;
    }
}

void ItemManager::Remove(Item *item)
{
    items.RemoveAt(item);
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

Item *ItemManager::GetItem(int index)
{
    return items.Get(index);
}