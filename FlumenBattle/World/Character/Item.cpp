#include "Item.h"
#include "FlumenBattle/World/Character/Types.h"

using namespace world::character;

#define ITEM_COUNT_PER_GROUP 64

/*class ModifierAccessor
{
public:
    static void AddModifier(Character &character, Modifier modifier)
    {
        character.AddModifier(modifier);
    }
};*/

class Sword : public ItemType
{
    using ItemType::ItemType; 

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
    }*/
};

class Axe : public ItemType
{
    using ItemType::ItemType; 

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
    }*/
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

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
    }*/
};

class Helmet : public ItemType
{
    using ItemType::ItemType; 

    /*void HandleApplyEffect(Character &character) const override
    {
        ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
    }*/
};

Item ItemFactory::Create(ItemTypes type)
{
    switch(type)
    {
    case ItemTypes::SWORD:
        return 
        {
            [&] {static const auto itemType = Sword(type, "SwordT1"); return &itemType;} (), 
            1,
            false
        };
    case ItemTypes::ARMOR:
        return 
        {
            [&] {static const auto itemType = Armor(type, "ArmorT1"); return &itemType;} (), 
            1,
            false
        };
    case ItemTypes::SHIELD:
        return 
        {
            [&] {static const auto itemType = Shield(type, "ShieldLargeT1"); return &itemType;} (), 
            1,
            false
        };
    case ItemTypes::HELMET:
        return 
        {
            [&] {static const auto itemType = Helmet(type, "HelmetT1"); return &itemType;} (), 
            1,
            false
        };
    case ItemTypes::AXE:
        return 
        {
            [&] {static const auto itemType = Axe(type, "AxeT1"); return &itemType;} (), 
            1,
            false
        };
    }
}

void ItemManager::Add(ItemTypes type)
{
    *items.Add() = ItemFactory::Get()->Create(type);
}

void ItemManager::Remove(Item *item)
{
    items.Remove(item);
}

int ItemManager::GetAmount(ItemTypes type)
{
    auto amount = 0;

    for(auto item : items)
    {
        if(item.Type->Type == type)
        {
            //amount += item.Amount;
            amount += 1;
        }
    }

    return amount;
}