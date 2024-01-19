#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "SettlementMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/SettlementMenuOption.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldController.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static constexpr auto DEFAULT_FONT_SIZE = "Medium";

static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

static constexpr auto OPTION_ITEM_SIZE = Size(280, 35);

void SettlementMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {Position2(), this}, 
            {"panel-border-031", "SlicedSprite"}, 
            Opacity(1.0f)
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildText
    (
        {
            Size(), 
            drawOrder_ + 1, 
            {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        {
            {DEFAULT_FONT_TYPE, DEFAULT_FONT_SIZE}, 
            Color::RED * 0.5f, 
            ""
        }
    );
    nameLabel->Enable();

    optionLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {Size(), drawOrder_, {Position2(10.0f, 70.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    optionLayout->Enable();

    auto optionItem = ElementFactory::BuildElement <SettlementMenuOption>
    (
        {
            OPTION_ITEM_SIZE, 
            drawOrder_ + 1, 
            {Position2(), optionLayout}, 
            {"panel-border-001", "SlicedSprite"}
        }
    );
    optionItem->SetSpriteColor(BORDER_COLOR);
    optionItem->SetInteractivity(true);
    optionItem->Setup(this);
    optionItem->Enable();
}

void SettlementMenu::HandleUpdate() 
{
    Phrase text;
    text << currentSettlement->GetName();
    nameLabel->Setup(text);
}

void SettlementMenu::Setup(settlement::Settlement *newSettlement)
{
    currentSettlement = newSettlement;
}

void SettlementMenu::ProcessOptionInput()
{
    if(WorldController::Get()->CanBuyFood() == true)
    {
        WorldController::Get()->BuyFood();
    }
}