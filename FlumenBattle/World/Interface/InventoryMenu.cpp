#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "InventoryMenu.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::interface;

#define MAXIMUM_SLOT_COUNT 12

static auto color = Color::RED * 0.25f;

world::group::Group *playerGroup = nullptr;

void InventorySlot::HandleConfigure()
{
    icon = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, {"SwordT1", "Sprite"}}
    );
    icon->Disable();

    border = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_ + 1, {Position2(0.0f, 0.0f), this}, {"panel-border-007", "SlicedSprite"}, Opacity(0.5f)}
    );
    border->GetSprite()->SetColor(&color);
    border->Disable();
}

void InventorySlot::HandleLeftClick()
{
    menu->SelectSlot(this);
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
    }
    else
    {
        icon->Disable();
    }
}

void InventoryMenu::HandleConfigure()
{
    playerGroup = world::WorldScene::Get()->GetPlayerGroup();

    nameLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Elric"}
    );
    nameLabel->AdjustSize();
    nameLabel->Enable();

    classLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 25.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, nameLabel}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Cleric"}
    );
    classLabel->AdjustSize();
    classLabel->Enable();

    Text **labels[] = {&healthLabel, &armorLabel, &attackLabel};
    auto height = 120.0f;
    for(auto label : labels)
    {
        *label = ElementFactory::BuildText(
            {Size(100, 100), drawOrder_ + 1, {Position2(10.0f, height), ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
            {{"JSLAncient", "Small"}, Color::RED * 0.5f, "Health: 10 / 10"}
        );
        (*label)->SetAlignment(Text::Alignments::LEFT);
        (*label)->AdjustSize();
        (*label)->Enable();

        height += (*label)->GetFontHeight() - 5.0f;
    }

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), this}, 
            {"panel-border-031", "SlicedSprite"}, 
            Opacity(1.0f)
        }
    );
    border->GetSprite()->SetColor(&color);
    border->Enable();

    slotLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        {
            Size(200, 200), 
            drawOrder_, 
            {Position2(0.0f, -20.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {"Sprite"}, 
            Opacity(0.0f)
        }
    );
    slotLayout->SetDistancing(6, 10.0f);
    slotLayout->Enable();

    inventorySlots.Initialize(MAXIMUM_SLOT_COUNT);
    for(int i = 0; i < MAXIMUM_SLOT_COUNT; ++i)
    {
        auto slot = ElementFactory::BuildElement <InventorySlot>
        (
            {Size(60, 60), drawOrder_ + 1, {Position2(0.0f, 0.0f), slotLayout}, {"panel-007", "SlicedSprite"}, Opacity(0.3f)}
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

        slotLayout->AddChild(slot);
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
            {Size(60, 60), drawOrder_ + 1, {slotData.Position, ElementAnchors::UPPER_RIGHT, ElementPivots::MIDDLE_CENTER, this}, {"panel-007", "SlicedSprite"}, Opacity(0.3f)}
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
        {size_, drawOrder_ + 3, {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, {"SwordT1", "Sprite"}}
    );
    grabbedItem->Disable();

    grabbedItem->FollowMouse();
}

void InventoryMenu::SelectSlot(InventorySlot *slot)
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

    character->RefreshModifiers();
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

void InventoryMenu::MoveItem(InventorySlot *slot)
{
    if(slot->isCharacterItem == true && selectedSlot->item->CanFitInto(slot->itemPosition) == false)
        return;

    if(slot->isCharacterItem == true)
    {
        selectedSlot->item->IsUsed = true;

        character->SetItem(selectedSlot->item, slot->itemPosition);
    }

    if(selectedSlot->isCharacterItem == true)
    {
        if(slot->isCharacterItem == false)
        {
            selectedSlot->item->IsUsed = false;
        }

        character->SetItem(nullptr, selectedSlot->itemPosition);
    }

    selectedSlot->Deselect();

    grabbedItem->Disable();

    slot->SetItem(selectedSlot->item);

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
        character->SetItem(selectedSlot->item, slot->itemPosition);
    }

    if(selectedSlot->isCharacterItem == true)
    {
        character->SetItem(slot->item, selectedSlot->itemPosition);
    }

    slot->Deselect();

    selectedSlot->Deselect();

    auto newItem = slot->item;

    slot->SetItem(selectedSlot->item);

    selectedSlot->SetItem(newItem);

    selectedSlot = nullptr;

    grabbedItem->Disable();
}

void InventoryMenu::SelectCharacter(character::Character *newCharacter)
{
    character = newCharacter;

    classLabel->Setup(character->GetClass()->Name.Get());

    nameLabel->Setup(character->GetName());

    mainHandSlot->SetItem(character->GetItem(character::ItemPositions::MAIN_HAND));
    bodySlot->SetItem(character->GetItem(character::ItemPositions::BODY));
    headSlot->SetItem(character->GetItem(character::ItemPositions::HEAD));
}

void InventoryMenu::HandleUpdate()
{
    Word text = "Health: ";
    text << character->GetCurrentHitPoints();

    healthLabel->Setup(text);

    text = "Armor: ";
    text << character->GetArmorClass();

    armorLabel->Setup(text);

    text = "Attack: ";
    text << character->GetAttackRating();

    attackLabel->Setup(text);
}