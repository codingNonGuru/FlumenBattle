#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "InventoryMenu.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

using namespace world::interface;

#define MAXIMUM_SLOT_COUNT 12

static auto color = Color::RED * 0.5f;

void InventoryMenu::HandleConfigure()
{
    characterLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Cleric"}
    );
    characterLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_, 
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
            {Size(60, 60), drawOrder_ + 1, {Position2(0.0f, 0.0f), slotLayout}, {"panel-transparent-center-019", "SlicedSprite"}, Opacity(0.3f)}
        );
        slot->GetSprite()->SetColor(&color);
        slot->Enable();

        *inventorySlots.Add() = slot;

        slotLayout->AddChild(slot);
    }
}

void InventoryMenu::SelectCharacter(character::Character *newCharacter)
{
    character = newCharacter;

    characterLabel->Setup(character->GetClass()->Name.Get());
}