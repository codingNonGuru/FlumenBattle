#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "ProductionDecisionItem.h"
#include "FlumenBattle/World/Interface/ProductionDecisionMenu.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"

using namespace world::interface;

static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

static constexpr auto OPTION_FONT_SIZE = "Small";

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void ProductionDecisionItem::HandleConfigure()
{
    label = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}
        },
        {
            {DEFAULT_FONT_TYPE, OPTION_FONT_SIZE}, 
            Color::RED * 0.5f
        }
    );
    label->Enable();
}

void ProductionDecisionItem::HandleLeftClick() 
{
    parent->ProcessInput(option);
}

void ProductionDecisionItem::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(HOVERED_OPTION_OPACITY);
    }
    else
    {
        SetOpacity(BASE_OPTION_OPACITY);
    }
}

void ProductionDecisionItem::Setup(ProductionDecisionMenu *newParent, settlement::ProductionOptions newOption)
{
    parent = newParent;

    option = newOption;

    label->Setup(settlement::SettlementProduction::GetName(option));
}
