#include "ItemHoverInfo.h"

#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "ItemHoverInfo.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Interface/InventoryMenu.h"
#include "FlumenBattle/World/Character/Item.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto BORDER_OPACITY = Opacity(0.7f);

static const auto BASE_SIZE = Size(100, 100);

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto NAME_LABEL_OFFSET = Position2(0.0f, 10.0f);

void ItemHoverInfo::HandleConfigure() 
{
    size_ = BASE_SIZE;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {NAME_LABEL_OFFSET, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Small"}, TEXT_COLOR, "Item"}
    );
    nameLabel->Enable();

    counterGroup = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {Position2(20.0f, 15.0f + nameLabel->GetSize().y + NAME_LABEL_OFFSET.y), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    //counterGroup->LockHeight(80.0f);
    counterGroup->SetDistancing(1, 30.0f);
    //counterGroup->SetOffset({-10.0f, 0.0f});
    counterGroup->Enable();

    valueCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {drawOrder_, {counterGroup}}
    );
    valueCounter->Enable();

    weightCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {drawOrder_, {counterGroup}}
    );
    weightCounter->SetOffset(5.0f);
    weightCounter->Enable();

    FollowMouse(MOUSE_FOLLOW_OFFSET);
}

void ItemHoverInfo::Setup(InventorySlot *hoveredSlot)
{
    this->hoveredSlot = hoveredSlot;

    this->item = hoveredSlot->GetItem();

    valueCounter->Setup(
        "Coin",
        &this->item->Type->Value
        //std::function <int(void)> ([] () -> int {return BattleInterface::Get()->GetHoveredCombatant()->GetCharacter()->GetArmorClass();}),
        );

    weightCounter->Setup(
        "CrateShadowed",
        &this->item->Type->Weight
        //std::function <int(void)> ([] () -> int {return BattleInterface::Get()->GetHoveredCombatant()->GetCharacter()->GetArmorClass();}),
        );
}

void ItemHoverInfo::HandleUpdate() 
{
    if(hoveredSlot == nullptr)
        return;

    if(hoveredSlot->IsHovered() == false)
    {
        hoveredSlot = nullptr;
        Disable();
        return;
    }

    Phrase text = item->Type->Name.Get();
    nameLabel->Setup(text);

    auto textWidth = nameLabel->GetSize().x + 10;
    if(textWidth > BASE_SIZE.x)
    {
        size_.x = textWidth;
    }
    else
    {
        size_.x = BASE_SIZE.x;
    }

    border->GetSize() = size_;
}