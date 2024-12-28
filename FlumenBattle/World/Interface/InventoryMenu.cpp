#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Delegate/Event.hpp"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "InventoryMenu.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Interface/ItemHoverInfo.h"

using namespace world::interface;

#define MAXIMUM_SLOT_COUNT 12

#define MAXIMUM_SKILL_ITEM_COUNT 6

static auto color = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

world::group::GroupCore *playerGroup = nullptr;

void InventorySlot::HandleConfigure()
{
    icon = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_ + 1, {this}, {"SwordT1", false}}
    );

    border = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_ + 1, {this}, {"panel-border-007", true}, Opacity(0.5f)}
    );
    border->GetSprite()->SetColor(&color);

    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 2, {Position2(-8.0f, -8.0f), ElementAnchors::LOWER_RIGHT, ElementPivots::LOWER_RIGHT, this}, {"WhiteDotBackdrop", false}}
    );
}

void InventorySlot::HandleLeftClick()
{
    menu->SelectSlot(this);
}

void InventorySlot::HandleHover()
{
    if(item == nullptr)
        return;

    auto itemHoverInfo = WorldInterface::Get()->GetItemHoverInfo();

    itemHoverInfo->Setup(this);

    itemHoverInfo->Enable();
}

void InventorySlot::Select()
{
    border->Enable();

    isSelected = true;

    icon->SetOpacity(0.5f);
}

void InventorySlot::Deselect()
{
    border->Disable();

    isSelected = false;

    icon->SetOpacity(1.0f);
}

void InventorySlot::SetItem(world::character::Item *newItem) 
{
    item = newItem;

    if(item != nullptr)
    {
        icon->GetSprite()->SetTexture(item->Type->TextureName);
        icon->Enable();

        if(item->Amount > 1)
        {
            counter->Setup(&item->Amount);
            counter->Enable();
        }
        else
        {
            counter->Disable();
        }
    }
    else
    {
        icon->Disable();
        counter->Disable();
    }
}

void InventoryMenu::HandleConfigure()
{
    group::HumanMind::Get()->OnItemAdded += {this, &InventoryMenu::HandleItemAdded};

    playerGroup = world::WorldScene::Get()->GetPlayerGroup();

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "Elric"}
    );
    nameLabel->Enable();

    classLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 25.0f), nameLabel}}, 
        {{"Medium"}, TEXT_COLOR, "Cleric"}
    );
    classLabel->Enable();

    levelProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(80, 24), drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, classLabel}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    levelProgress->Enable();

    Text **labels[] = {&healthLabel, &armorLabel, &attackLabel, &damageLabel};
    auto height = 90.0f;
    for(auto label : labels)
    {
        *label = ElementFactory::BuildText(
            {Size(100, 100), drawOrder_ + 1, {Position2(10.0f, height), ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
            {{"Small"}, Color::RED * 0.5f, "Health: 10 / 10"}
        );
        (*label)->SetAlignment(Text::Alignments::LEFT);
        (*label)->Enable();

        height += (*label)->GetFontHeight() - 5.0f;
    }

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->GetSprite()->SetColor(&color);
    border->Enable();

    slotLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2(0.0f, -20.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {false}
        }
    );
    slotLayout->SetDistancing(6, 10.0f);
    slotLayout->Enable();

    inventorySlots.Initialize(MAXIMUM_SLOT_COUNT);
    for(int i = 0; i < MAXIMUM_SLOT_COUNT; ++i)
    {
        auto slot = ElementFactory::BuildElement <InventorySlot>
        (
            {Size(60, 60), drawOrder_ + 1, {slotLayout}, {"panel-007", true}, Opacity(0.3f)}
        );
        slot->GetSprite()->SetColor(&color);

        auto item = playerGroup->GetItem(i);
        if(item != nullptr && item->IsUsed == false)
        {
            slot->SetItem(item);
        }

        slot->menu = this;

        slot->SetInteractivity(true);
        slot->Enable();

        *inventorySlots.Add() = slot;
    }

    struct SlotData
    {
        InventorySlot **Slot;

        Position2 Position;

        world::character::ItemPositions Use;
    };

    SlotData characterSlots[] = {
        {&mainHandSlot, Position2(-170.0f, 140.0f), character::ItemPositions::MAIN_HAND}, 
        {&bodySlot, Position2(-70.0f, 190.0f), character::ItemPositions::BODY}, 
        {&headSlot, Position2(-80.0f, 100.0f), character::ItemPositions::HEAD}
        };

    for(auto slotData : characterSlots)
    {
        *slotData.Slot = ElementFactory::BuildElement <InventorySlot>
        (
            {Size(60, 60), drawOrder_ + 1, {slotData.Position, ElementAnchors::UPPER_RIGHT, ElementPivots::MIDDLE_CENTER, this}, {"panel-007", true}, Opacity(0.3f)}
        );

        auto slot = *slotData.Slot;
        slot->GetSprite()->SetColor(&color);

        slot->SetItem(nullptr);

        slot->menu = this;
        slot->isCharacterItem = true;
        slot->itemPosition = slotData.Use;

        slot->SetInteractivity(true);
        slot->Enable();
    }

    grabbedItem = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_ + 3, {this}, {"SwordT1", false}}
    );
    grabbedItem->Disable();

    grabbedItem->FollowMouse();

    weightCounter = ElementFactory::BuildElement <ResourceCounter>(
        {drawOrder_ + 1, {Position2(-30.0f, -20.0f), ElementAnchors::UPPER_CENTER, ElementPivots::MIDDLE_CENTER, nullptr}}
    );
    weightCounter->SetDynamicParent(slotLayout);
    auto fetcher = [] () -> Word {
        auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
        Word text = "";
        text << playerGroup->GetCarriedWeight();
        text << " / " << playerGroup->GetCarryCapacity();
        return text;
    };
    weightCounter->Setup("CrateShadowed", std::function <Word(void)> (fetcher));
    weightCounter->SetOffset(7.0f);
    weightCounter->Enable();

    skillLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(20.0f, 175.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "Skills"}
    );
    skillLabel->Enable();

    skillItemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2(-10.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, skillLabel}
        }
    );
    skillItemLayout->SetDistancing(1, 32.0f);
    skillItemLayout->Enable();

    skillItems.Initialize(MAXIMUM_SKILL_ITEM_COUNT);
    for(int i = 0; i < skillItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <SkillItem>
        (
            {drawOrder_ + 1, {skillItemLayout}, {false}, Opacity(0.3f)}
        );
        item->Enable();
        //item->SetInteractivity(true);

        *skillItems.Add() = item;
    }
}

void InventoryMenu::SelectSlot(InventorySlot *slot)
{
    if(selectedSlot == nullptr && slot->item == nullptr)
        return;

    if(group::HumanMind::Get()->IsSellModeActive() == true)
    {
        if(selectedSlot == nullptr && slot->isCharacterItem == false)
        {
            SellItem(slot);
        }
    }
    else
    {
        if(selectedSlot == slot)
        {
            DropItem();
        }
        else
        {
            if(slot->item != nullptr)
            {
                if(selectedSlot == nullptr)
                {
                    GrabItem(slot);
                }
                else 
                {
                    SwapItem(slot);
                }
            }
            else 
            {
                MoveItem(slot);
            }
        }
    }
}

void InventoryMenu::HandleEnable()
{
    for(auto &slot : inventorySlots)
    {
        if(slot->item == nullptr)
            continue;

        if(slot->item->Amount == 0 || slot->item->IsUsed == true)
        {
            slot->SetItem(nullptr);
        }
        else
        {
            slot->SetItem(slot->item);
        }
    }
}

void InventoryMenu::DropItem()
{
    selectedSlot->Deselect();
    selectedSlot = nullptr;

    grabbedItem->Disable();
}

void InventoryMenu::GrabItem(InventorySlot *slot)
{
    selectedSlot = slot;

    selectedSlot->Select();

    grabbedItem->Enable();

    grabbedItem->GetSprite()->SetTexture(slot->item->Type->TextureName);
}

void InventoryMenu::MoveItem(InventorySlot *destinationSlot)
{
    if(destinationSlot->isCharacterItem == true && selectedSlot->item->CanFitInto(destinationSlot->itemPosition) == false)
        return;

    if(destinationSlot->isCharacterItem == true)
    {
        character->EquipItem(selectedSlot->item, destinationSlot->itemPosition);
    }

    if(selectedSlot->isCharacterItem == true)
    {
        if(destinationSlot->isCharacterItem == false)
        {
            //selectedSlot->item->IsUsed = false;
        }

        character->UnequipItem(selectedSlot->itemPosition);
    }

    selectedSlot->Deselect();

    grabbedItem->Disable();

    destinationSlot->SetItem(selectedSlot->item);

    selectedSlot->SetItem(nullptr);

    selectedSlot = nullptr;
}

void InventoryMenu::SwapItem(InventorySlot *slot)
{
    if(slot->isCharacterItem == true && selectedSlot->item->CanFitInto(slot->itemPosition) == false)
        return;

    if(selectedSlot->isCharacterItem == true && slot->item->CanFitInto(selectedSlot->itemPosition) == false)
        return;

    if(slot->isCharacterItem == true)
    {
        character->EquipItem(selectedSlot->item, slot->itemPosition);
    }

    if(selectedSlot->isCharacterItem == true)
    {
        character->EquipItem(slot->item, selectedSlot->itemPosition);
    }

    slot->Deselect();

    selectedSlot->Deselect();

    auto newItem = slot->item;

    slot->SetItem(selectedSlot->item);

    selectedSlot->SetItem(newItem);

    selectedSlot = nullptr;

    grabbedItem->Disable();
}

void InventoryMenu::SellItem(InventorySlot *slot)
{
    auto item = slot->item;

    slot->SetItem(nullptr);

    group::HumanMind::Get()->SellItem(item);
}

void InventoryMenu::SelectCharacter(character::Character *newCharacter)
{
    character = newCharacter;

    nameLabel->Setup(character->GetName());

    LongWord description = LongWord("Level ") << character->GetLevel() << " ";
    description << character->GetRace()->Name;
    description << " ";
    description << character->GetClass()->Name;
    classLabel->Setup(description);

    levelProgress->SetProgress(character->GetLevelProgress());

    mainHandSlot->SetItem(character->GetItem(character::ItemPositions::MAIN_HAND));
    bodySlot->SetItem(character->GetItem(character::ItemPositions::BODY));
    headSlot->SetItem(character->GetItem(character::ItemPositions::HEAD));

    for(auto &item : skillItems)
        item->Disable();

    auto item = skillItems.GetStart();
    for(int i = 0; i < (int)character::SkillTypes::NONE; ++i)
    {
        auto isTrained = newCharacter->IsTrainedInSkill(character::SkillTypes(i));
        if(isTrained == false)
            continue;

        (*item)->Setup(character, character::SkillTypes(i));
        (*item)->Enable();

        item++;
    }
}

void InventoryMenu::HandleUpdate()
{
    Word text = "Health: ";
    text << character->GetCurrentHitPoints();
    text << " / ";
    text << character->GetMaximumHitPoints();

    healthLabel->Setup(text);

    text = "Armor: ";
    text << character->GetArmorClass();

    armorLabel->Setup(text);

    text = "Attack: ";
    auto attackRating = character->GetAttackRating();
    if(attackRating > 0)
    {
        text << "+";
    }
    text << attackRating;

    attackLabel->Setup(text);

    text = "Damage: ";
    auto damage = character->GetDamage();
    if(damage.Bonus > 0)
    {
        text << damage.Bonus;
        text << " + ";
    }
    text << damage.DieCount;
    text << "d";
    switch(damage.Die)
    {
    case utility::RollDies::D4:
        text << "4";
        break;
    case utility::RollDies::D6:
        text << "6";
        break;
    case utility::RollDies::D8:
        text << "8";
        break;
    case utility::RollDies::D10:
        text << "10";
        break;
    case utility::RollDies::D12:
        text << "12";
        break;
    }

    damageLabel->Setup(text);
}

void InventoryMenu::HandleItemAdded()
{
    auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

    for(auto &item : playerGroup->GetItems())
    {
        if(item.IsUsed == true)
            continue;

        bool hasFound = false;
        for(auto &slot : inventorySlots)
        {
            if(slot->item == &item)
            {
                hasFound = true;
                break;
            }
        }

        if(hasFound == false)
        {
            for(auto &slot : inventorySlots)
            {
                if(slot->item != nullptr)
                    continue;

                slot->SetItem(&item);
                break;    
            }
            break;
        }
    }
}

void SkillItem::HandleConfigure()
{
    counter = ElementFactory::BuildElement <ResourceCounter>(
        {drawOrder_ + 1, {this}}
    );

    counter->Setup("CrateShadowed", &skillBonus);
    counter->Enable();

    counter->MakeSignSensitive();
}

void SkillItem::Setup(character::Character *character, character::SkillTypes skillType)
{
    this->character = character;
    this->skillType = skillType;

    skillBonus = character->GetSkillProficiency(skillType);

    counter->SetIconTexture(character->GetSkillTexture(skillType));
}