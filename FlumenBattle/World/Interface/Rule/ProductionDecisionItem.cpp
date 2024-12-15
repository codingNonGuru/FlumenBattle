#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "ProductionDecisionItem.h"
#include "FlumenBattle/World/Interface/Rule/ProductionDecisionInterface.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::interface::rule;

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
            {OPTION_FONT_SIZE}, 
            Color::RED * 0.5f
        }
    );
    label->Enable();

    laborCounter = ElementFactory::BuildElement <ResourceCounter> (
        {drawOrder_, {Position2(-80.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );
    laborCounter->Enable();

    laborCounter->SetOffset(5.0f);
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

void ProductionDecisionItem::Setup(ProductionDecisionInterface *newParent, settlement::ProductionOptions newOption)
{
    parent = newParent;

    option = newOption;

    label->Setup(settlement::SettlementProduction::GetName(option));

    laborCounter->Setup("WorkHammer", &settlement::SettlementProduction::GetType(option)->Cost);
}
