#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "DomainHoverInfo.h"
#include "RealmTab.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/CounterSet.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto OPACITY = Opacity(0.7f);

static const auto BORDER_OPACITY = Opacity(0.7f);

static const auto BASE_SIZE = Size(200, 100);

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto NAME_LABEL_OFFSET = Position2(0.0f, 10.0f);

void DomainHoverInfo::HandleConfigure()
{
    size_ = BASE_SIZE;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {NAME_LABEL_OFFSET, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Small"}, TEXT_COLOR, HIGHLIGHT_COLOR}
    );
    nameLabel->Enable();

    resourceCounterSet = ElementFactory::BuildElement <CounterSet>
    (
        {
            Size(), 
            drawOrder_, 
            {Position2(0.0f, 30.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    resourceCounterSet->Enable();

    FollowMouse(MOUSE_FOLLOW_OFFSET);

    opacity_ = OPACITY;
}

void DomainHoverInfo::HandleUpdate()
{
    if(hoveredItem == nullptr)
        return;

    if(hoveredItem->IsHovered() == false)
    {
        hoveredItem = nullptr;
        Disable();
        return;
    }

    auto text = Word("<2>") << settlement->GetName().GetFirstCharacter() << "<1>" << (settlement->GetName().Get() + 1);
    nameLabel->Setup(text);
}

void DomainHoverInfo::Setup(DomainItem *item)
{
    this->hoveredItem = item;

    this->settlement = item->GetSettlement();

    auto &resourceData = this->settlement->GetTileResourceData();

    resourceCounterSet->SetDistanceFactor(7.0f);
    resourceCounterSet->SetItemParameters(Position2{10.0f, 12.0f}, 0.7f);
    resourceCounterSet->Setup(resourceData);
}