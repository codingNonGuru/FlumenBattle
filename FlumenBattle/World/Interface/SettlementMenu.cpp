#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SettlementMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static constexpr auto DEFAULT_FONT_SIZE = "Medium";

static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

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