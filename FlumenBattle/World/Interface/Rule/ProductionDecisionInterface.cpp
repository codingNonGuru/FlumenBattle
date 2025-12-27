#include <stdarg.h>

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/SimpleList.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "ProductionDecisionInterface.h"
#include "FlumenBattle/World/Interface/Rule/ProductionDecisionItem.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/WorldTime.h"

using namespace world;
using namespace world::interface;
using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static constexpr auto DEFAULT_FONT_SIZE = "Medium";

static constexpr auto OPTION_LAYOUT_POSITION = Position2(0.0f, 70.0f);

static const container::Array <settlement::ProductionOptions> recruitmentOptions = {
    settlement::ProductionOptions::SETTLERS, 
    settlement::ProductionOptions::PATROL,
    settlement::ProductionOptions::GARRISON
    };

static const container::Array <settlement::ProductionOptions> buildingOptions = {
    settlement::ProductionOptions::WALLS, 
    settlement::ProductionOptions::IRRIGATION, 
    settlement::ProductionOptions::LIBRARY,
    settlement::ProductionOptions::SEWAGE,
    settlement::ProductionOptions::HOUSING,
    settlement::ProductionOptions::LUMBER_MILL,
    settlement::ProductionOptions::CARPENTER,
    settlement::ProductionOptions::BAKERY,
    settlement::ProductionOptions::MARKET,
    settlement::ProductionOptions::WEAVING_MILL,
    settlement::ProductionOptions::TAILORY,
    settlement::ProductionOptions::KEEP,
    settlement::ProductionOptions::POTTERY
    };

#define ITEM_CAPACITY std::size(options)

void ProductionDecisionInterface::HandleConfigure(AdditionalElementData *additionalData)
{
    productionLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        {
            {"Small"}, 
            TEXT_COLOR
        }
    );
    productionLabel->Enable();

    productionProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(160, 32), drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, productionLabel}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );

    timeLeftLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 3, 
            {productionProgress}
        },
        {
            {"VerySmall"}, 
            Color::WHITE
        }
    );
    timeLeftLabel->Enable();

    laborCounter = ElementFactory::BuildElement <ResourceCounter> (
        {drawOrder_ + 1, {Position2(10.0f, 30.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    laborCounter->Enable();

    laborCounter->Setup("WorkHammer", &industrialCapacity, "VeryLarge", Scale2(1.25f));
    laborCounter->SetOffset(5.0f);

    auto data = (ProductionInterfaceData *)additionalData;

    productionType = data->Type;

    auto maximumItemCount = productionType == settlement::ProductionClasses::RECRUITMENT ? recruitmentOptions.GetSize() : buildingOptions.GetSize();

    optionLayout = ElementFactory::BuildSimpleList
    (
        {drawOrder_ + 5, {OPTION_LAYOUT_POSITION, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        maximumItemCount,
        ListOrientations::VERTICAL,
        5.0f
    );
    optionLayout->AddScroller(6, maximumItemCount);
    optionLayout->Enable();

    static const auto OPTION_ITEM_SIZE = Size(size_.x, 35);

    optionItems.Initialize(maximumItemCount);
    for(auto i = 0; i < optionItems.GetCapacity(); ++i)
    {
        auto optionItem = ElementFactory::BuildElement <ProductionDecisionItem>
        (
            {
                OPTION_ITEM_SIZE, 
                drawOrder_ + 2, 
                {optionLayout}, 
                {"panel-border-001", true}
            }
        );
        optionItem->SetSpriteColor(BORDER_COLOR);
        optionItem->SetInteractivity(true);

        *optionItems.Add() = optionItem;
    }
}

void ProductionDecisionInterface::HandleUpdate()
{
    for(auto &item : optionItems)
    {
        item->Disable();
    }

    int validOptionCount = 0;

    auto &options = (productionType == settlement::ProductionClasses::RECRUITMENT ? recruitmentOptions : buildingOptions);

    auto item = optionItems.GetStart();
    for(auto &option : options)
    {
        auto inquiry = settlement::SettlementProduction::CanProduce(*currentSettlement, option);
        if(inquiry.CanProduce == false)
            continue;

        (*item)->Setup(this, option);
        (*item)->Enable();

        item++;

        validOptionCount++;
    }

    optionLayout->SetScrollableChildCount(validOptionCount);

    const auto currentProduction = [&] 
    {
        if(productionType == settlement::ProductionClasses::BUILDING)
            return currentSettlement->GetBuildingProduction();
        else if(productionType == settlement::ProductionClasses::RECRUITMENT)
            return currentSettlement->GetGroupProduction();
    } ();

    auto text = Word() << (productionType == settlement::ProductionClasses::BUILDING ? "Building " : "Recruiting ") << currentProduction->GetName();

    productionLabel->Setup(text);

    industrialCapacity = productionType == settlement::ProductionClasses::BUILDING ? currentSettlement->GetIndustrialProduction() : currentSettlement->GetRecruitmentCapacity();

    if(currentProduction->Is(settlement::ProductionOptions::NONE) == false)
    {
        productionProgress->Enable();

        productionProgress->SetProgress(currentProduction->GetProgressRatio());

        if(industrialCapacity == 0)
        {
            timeLeftLabel->Setup(Word() << "-");
        }
        else
        {
            auto tickCount = currentProduction->GetRemainingProgress() / industrialCapacity;
            tickCount /= WorldTime::HOUR_SIZE;

            timeLeftLabel->Setup(Word() << tickCount << " hrs");
        }
    }
    else
    {
        productionProgress->Disable();
    }
}

void ProductionDecisionInterface::Setup(settlement::Settlement *settlement)
{
    currentSettlement = settlement;
}

void ProductionDecisionInterface::ProcessInput(settlement::ProductionOptions option)
{
    polity::HumanMind::Get()->ProcessProductionInput(option, productionType, currentSettlement);
}