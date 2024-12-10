#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "ProductionDecisionMenu.h"
#include "FlumenBattle/World/Interface/ProductionDecisionItem.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world;
using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static constexpr auto DEFAULT_FONT_SIZE = "Medium";

static constexpr auto OPTION_LAYOUT_POSITION = Position2(10.0f, 80.0f);

static const settlement::ProductionOptions options[] = {
    settlement::ProductionOptions::WALLS,
    settlement::ProductionOptions::PATROL, 
    settlement::ProductionOptions::GARRISON,
    settlement::ProductionOptions::SETTLERS, 
    settlement::ProductionOptions::IRRIGATION, 
    settlement::ProductionOptions::LIBRARY,
    settlement::ProductionOptions::SEWAGE,
    settlement::ProductionOptions::HOUSING,
    settlement::ProductionOptions::FARM,
    settlement::ProductionOptions::LUMBER_MILL,
    settlement::ProductionOptions::CARPENTER
    };

#define ITEM_CAPACITY std::size(options)

void ProductionDecisionMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        {
            {DEFAULT_FONT_SIZE}, 
            TEXT_COLOR
        }
    );
    nameLabel->Enable();

    productionLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nameLabel}
        },
        {
            {"Small"}, 
            TEXT_COLOR
        }
    );
    productionLabel->Enable();

    productionProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(160, 16), drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, productionLabel}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 8.0f}}
    );

    laborCounter = ElementFactory::BuildElement <ResourceCounter> (
        {drawOrder_ + 1, {Position2(30.0f, 30.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    laborCounter->Enable();

    laborCounter->Setup("WorkHammer", &industrialCapacity, "VeryLarge", Scale2(1.25f));
    laborCounter->SetOffset(5.0f);

    optionLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {OPTION_LAYOUT_POSITION, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    optionLayout->SetDistancing(1, 5.0f);
    optionLayout->Enable();

    static const auto OPTION_ITEM_SIZE = Size(size_.x - OPTION_LAYOUT_POSITION.x * 2, 35);

    optionItems.Initialize(ITEM_CAPACITY);
    for(auto i = 0; i < optionItems.GetCapacity(); ++i)
    {
        auto optionItem = ElementFactory::BuildElement <ProductionDecisionItem>
        (
            {
                OPTION_ITEM_SIZE, 
                drawOrder_ + 1, 
                {optionLayout}, 
                {"panel-border-001", true}
            }
        );
        optionItem->SetSpriteColor(BORDER_COLOR);
        optionItem->SetInteractivity(true);

        *optionItems.Add() = optionItem;
    }
}

void ProductionDecisionMenu::HandleUpdate()
{
    for(auto &item : optionItems)
    {
        item->Disable();
    }

    auto item = optionItems.GetStart();
    for(auto &option : options)
    {
        auto inquiry = settlement::SettlementProduction::CanProduce(*currentSettlement, option);
        if(inquiry.CanProduce == false)
            continue;

        (*item)->Setup(this, option);
        (*item)->Enable();

        item++;
    }

    const auto currentProduction = currentSettlement->GetCurrentProduction();

    auto text = Word() << "Building " << currentProduction->GetName();

    productionLabel->Setup(text);

    if(currentProduction->Is(settlement::ProductionOptions::NONE) == false)
    {
        productionProgress->Enable();

        productionProgress->SetProgress(currentProduction->GetProgressRatio());
    }
    else
    {
        productionProgress->Disable();
    }

    industrialCapacity = currentSettlement->GetIndustrialProduction();
}

void ProductionDecisionMenu::Setup(settlement::Settlement *settlement)
{
    currentSettlement = settlement;

    nameLabel->Setup(currentSettlement->GetName());
}

void ProductionDecisionMenu::ProcessInput(settlement::ProductionOptions option)
{
    polity::HumanMind::Get()->ProcessProductionInput(option, currentSettlement);
}