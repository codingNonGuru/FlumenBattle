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
        if(item != nullptr)
        {
            slot->SetItem(item);
        }

        slot->menu = this;

        slot->SetInteractivity(true);
        slot->Enable();

        *inventorySlots.Add() = slot;

        slotLayout->AddChild(slot);
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
        selectedSlot->Deselect();
        selectedSlot = nullptr;

        grabbedItem->Disable();
    }
    else
    {
        if(slot->item != nullptr)
        {
            if(selectedSlot == nullptr)
            {
                selectedSlot = slot;

                selectedSlot->Select();

                grabbedItem->Enable();

                grabbedItem->GetSprite()->SetTexture(slot->item->Type->TextureName);
            }
            else
            {
                slot->Deselect();

                selectedSlot->Deselect();

                auto newItem = slot->item;

                slot->SetItem(selectedSlot->item);

                selectedSlot->SetItem(newItem);

                selectedSlot = nullptr;

                grabbedItem->Disable();
            }
        }
        else
        {
            selectedSlot->Deselect();

            grabbedItem->Disable();

            slot->SetItem(selectedSlot->item);

            selectedSlot->SetItem(nullptr);

            selectedSlot = nullptr;
        }
    }
}

void InventoryMenu::SelectCharacter(character::Character *newCharacter)
{
    character = newCharacter;

    classLabel->Setup(character->GetClass()->Name.Get());

    nameLabel->Setup(character->GetName());
}

void InventoryMenu::HandleUpdate()
{
    if(selectedSlot == nullptr)
        return;
}