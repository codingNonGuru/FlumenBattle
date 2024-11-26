#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "BuildingHoverInfo.h"
#include "FlumenBattle/World/Interface/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Building.h"

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto OPACITY = Opacity(0.7f);

static const auto BORDER_OPACITY = Opacity(0.7f);

static const auto BASE_SIZE = Size(100, 100);

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto NAME_LABEL_OFFSET = Position2(0.0f, 10.0f);

using namespace world::interface;

void BuildingHoverInfo::HandleConfigure() 
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

    FollowMouse(MOUSE_FOLLOW_OFFSET);

    opacity_ = OPACITY;
}

void BuildingHoverInfo::Setup(BuildingItem *hoveredItem)
{
    this->hoveredItem = hoveredItem;

    this->building = hoveredItem->GetBuilding();
}

void BuildingHoverInfo::HandleUpdate() 
{
    if(hoveredItem == nullptr)
        return;

    if(hoveredItem->IsHovered() == false)
    {
        hoveredItem = nullptr;
        Disable();
        return;
    }

    Word text = building->GetName();
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

    size_.y = nameLabel->GetSize().y + NAME_LABEL_OFFSET.y * 2.0f;

    border->GetSize() = size_;
}