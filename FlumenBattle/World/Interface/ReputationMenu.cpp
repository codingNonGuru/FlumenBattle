#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "ReputationMenu.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/ReputationHandler.h"
#include "FlumenBattle/World/Group/Reputation.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

#define MAXIMUM_ITEM_COUNT 8

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto ITEM_LIST_OFFSET = Position2(15.0f, 70.0f);

void ReputationMenu::HandleConfigure() 
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    titleLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Large"}, Color::RED * 0.5f, "Reputation"}
    );
    titleLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {ITEM_LIST_OFFSET, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}
        }
    );
    itemLayout->SetDistancing(1, 10.0f);
    itemLayout->Enable();

    items.Initialize(MAXIMUM_ITEM_COUNT);

    for(auto i = 0; i < MAXIMUM_ITEM_COUNT; ++i)
    {
        auto item = items.Add();

        *item = ElementFactory::BuildText(
            {Size(), drawOrder_ + 1, {itemLayout}},
            {{"Small"}, Color::RED * 0.5f, "Aloha"}
        );
    }
}

void ReputationMenu::HandleUpdate()
{
    for(auto &item : items)
    {
        item->Disable();
    }

    auto &reputationHandler = group::HumanMind::Get()->GetPlayerReputation();

    auto item = items.GetStart();
    for(auto &reputation : reputationHandler.GetReputations())
    {
        (*item)->Enable();

        auto text = Phrase() << reputation.Settlement->GetName() << " " << reputation.GetSum();
        (*item)->Setup(text);

        item++;
    }
}