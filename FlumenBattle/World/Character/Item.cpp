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

class Spear : public ItemType
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
            [&] {static const auto itemType = Armor(type, "SpearT1", ItemUses::WEAPON); return &itemType;} ()
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

Item *ItemManager::GetItem(int index)
{
    return items.Get(index);
}