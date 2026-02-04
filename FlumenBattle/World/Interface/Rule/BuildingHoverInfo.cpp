#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "BuildingHoverInfo.h"
#include "FlumenBattle/World/Interface/Rule/BuildingTab.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto OPACITY = Opacity(0.7f);

static const auto BORDER_OPACITY = Opacity(0.7f);

static const auto BASE_SIZE = Size(100, 100);

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto NAME_LABEL_OFFSET = Position2(0.0f, 10.0f);

using namespace world::interface::rule;

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
        {{"Small"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    workerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nameLabel}}, 
        {{"VerySmall"}, TEXT_COLOR}
    );

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

    auto text = LongWord() << building->GetName();
    nameLabel->Setup(text);

    /*auto instructions = polity::HumanMind::Get()->GetSettlementInstructions();
    if(instructions == nullptr)
    {
        workerLabel->Disable();
    }
    else
    {
        text = "Worker priorities:\n";

        auto i = 0;
        for(auto &instruction : *instructions)
        {
            if(instruction.PlaceType != polity::WorkInstruction::BUILDING)
                continue;

            if(instruction.Place.Building != building)
                continue;

            if(i != 0)
            {
                text << ", ";
            }

            text << instruction.Priority + 1;

            i++;
        }

        if(i != 0)
        {
            workerLabel->Setup(text);
            workerLabel->Enable();
        }
        else
        {
            workerLabel->Disable();
        }
    }*/

    if(workerLabel->IsLocallyActive() == true)
    {
        size_.y = nameLabel->GetHeight() + workerLabel->GetHeight() + NAME_LABEL_OFFSET.y * 2.0f;
    }
    else
    {
        size_.y = nameLabel->GetHeight() + NAME_LABEL_OFFSET.y * 2.0f;
    }

    auto textWidth = workerLabel->IsLocallyActive() == true ? workerLabel->GetWidth() : nameLabel->GetWidth();
    textWidth += 10;

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