#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "EconomyTab.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/Rule/BuildingHoverInfo.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Config.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_BUILDING_COUNT 16

void ResourceItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(10.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, 
            {"Radish", false}
        }
    );
    icon->Enable();

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(50.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "Wool"}
    );
    nameLabel->Enable();

    storedLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(200.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    storedLabel->Enable();

    outputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(280.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    outputLabel->Enable();

    inputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(360.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    inputLabel->Enable();

    needBar = nullptr;
}

void ResourceItem::HandleUpdate()
{
    auto text = Word() << resource->Storage;

    storedLabel->Setup(text);

    text = Word("+") << resource->Production;

    outputLabel->Setup(text);

    text = Word() << resource->Order;

    inputLabel->Setup(text);

    if(needBar != nullptr)
    {
        static const auto TICKS_PER_NEED_SATISFACTION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TICKS_PER_NEED_SATISFACTION).Integer;

        auto satisfaction = settlement->GetNeedSatisfaction(resource->Type->Type);

        auto progress = (float)satisfaction / (float)TICKS_PER_NEED_SATISFACTION;

        needBar->SetProgress(progress);

        needLabel->Setup(ShortWord() << satisfaction);
    }
}

void ResourceItem::Setup(const settlement::Resource *resource, const settlement::Settlement *settlement)
{
    this->resource = resource;

    this->settlement = settlement;

    nameLabel->Setup(resource->Type->Name);

    icon->SetTexture(resource->Type->TextureName);

    bool relatesToNeed = 
        resource->Type->Type == settlement::ResourceTypes::COOKED_FOOD || 
        resource->Type->Type == settlement::ResourceTypes::FURNITURE || 
        resource->Type->Type == settlement::ResourceTypes::FOOD;

    if(needBar == nullptr && relatesToNeed == true)
    {
        needBar = ElementFactory::BuildProgressBar <ProgressBar>(
            {Size(60, 24), drawOrder_ + 1, {Position2(-10.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}, {"BaseBar", true}},
            {"BaseFillerRed", {6.0f, 6.0f}}
        );
        needBar->Enable();

        needLabel = ElementFactory::BuildText(
            {drawOrder_ + 2, {needBar}}, 
            {{"VerySmall"}, Color::WHITE}
        );
        needLabel->Enable();
    }
}

void BuildingItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {this}, 
            {"Radish", false}
        }
    );
    icon->SetTextureScale(Scale2(0.8f));
    icon->Enable();

    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(-8.0f, -8.0f), ElementAnchors::LOWER_RIGHT, ElementPivots::LOWER_RIGHT, this}, {"WhiteDotBackdrop", false}}
    );
    counter->Setup(&buildingCount, Scale2(1.0f), "Medium");

    workerCounter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(-8.0f, 8.0f), ElementAnchors::UPPER_RIGHT, ElementPivots::UPPER_RIGHT, this}, {"WhiteDotBackdrop", false}}
    );
    workerCounter->Setup(&workerCount, Scale2(0.8f), "Small");
}

void BuildingItem::HandleUpdate()
{
    if(this->building == nullptr)
    {
        counter->Disable();
        return;
    }

    buildingCount = building->GetAmount();

    if(buildingCount == 1)
    {
        counter->Disable();
    }
    else
    {
        counter->Enable();
    }

    workerCount = building->GetPersonnelCount();

    if(building->GetOutputResource().Resource != world::settlement::ResourceTypes::NONE)
    {
        workerCounter->Enable();
    }
    else
    {
        workerCounter->Disable();
    }
}

void BuildingItem::HandleHover()
{
    if(building == nullptr)
        return;

    auto hoverInfo = parentTab->GetHoverDevice();

    hoverInfo->Setup(this);

    hoverInfo->Enable();
}

void BuildingItem::HandleLeftClick()
{
    if(building == nullptr)
        return;

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->GetCurrentSettlement()->HireWorker(building);
}

void BuildingItem::HandleRightClick()
{
    if(building == nullptr)
        return;

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->GetCurrentSettlement()->FireWorker(building);
}

void BuildingItem::Setup(settlement::Building *building, EconomyTab *tab)
{
    this->building = building;

    this->parentTab = tab;

    if(building != nullptr)
    {
        icon->SetTexture(building->GetTextureName());
        icon->Enable();
    }
    else
    {
        icon->Disable();
    }
}

void EconomyTab::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    populationLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nameLabel}}, 
        {{"Small"}, TEXT_COLOR}
    );
    populationLabel->Enable();

    workerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, populationLabel}}, 
        {{"Small"}, TEXT_COLOR}
    );
    workerLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, -30.0f}, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    itemLayout->SetDistancing(1, 5.0f);
    itemLayout->Enable();

    resourceItems.Initialize((int)settlement::ResourceTypes::NONE);
    for(auto i = 0; i < (int)settlement::ResourceTypes::NONE; ++i)
    {
        auto item = ElementFactory::BuildElement <ResourceItem>
        (
            {
                Size(500, 35), 
                drawOrder_ + 1, 
                {itemLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *resourceItems.Allocate() = item;
    }

    buildingLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_,
            {Position2{0.0f, -10.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {this},
            Opacity(0.0f)
        }
    );
    buildingLayout->SetDistancing(MAXIMUM_BUILDING_COUNT / 2, 5.0f, 5.0f);
    buildingLayout->Enable();

    buildingItems.Initialize(MAXIMUM_BUILDING_COUNT);
    for(auto i = 0; i < buildingItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <BuildingItem>
        (
            {
                Size(64, 64), 
                drawOrder_ + 1, 
                {buildingLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *buildingItems.Allocate() = item;
    }

    buildingHoverInfo = ElementFactory::BuildElement <interface::rule::BuildingHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &EconomyTab::HandleSettlementChanged};
}

void EconomyTab::HandleUpdate()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    auto settlement = ruleMenu->GetCurrentSettlement();

    if(settlement == nullptr)
        return;

    auto item = buildingItems.GetStart();
    for(auto &building : settlement->GetBuildings())
    {
        (*item)->Setup(&building, this);

        (*item)->SetInteractivity(true);
        
        item++;
    }

    while(item != buildingItems.GetEnd())
    {
        (*item)->Setup(nullptr, nullptr);

        (*item)->SetInteractivity(false);

        item++;
    }

    nameLabel->Setup(settlement->GetName());

    auto text = Phrase() << settlement->GetPopulation() << " " << settlement->GetRace()->PluralName << " live in this jolly settlement, part of the realm of " << settlement->GetRuler()->GetName() << ".";
    populationLabel->Setup(text);

    text = Phrase() << settlement->GetName() << " has " << settlement->GetFreeWorkerCount() << " free workers.";
    workerLabel->Setup(text);
}

void EconomyTab::HandleEnable()
{

}

void EconomyTab::HandleSettlementChanged() 
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        nameLabel->Disable();

        for(auto &item : resourceItems)
        {
            item->Disable();
        }

        for(auto &item : buildingItems)
        {
            item->Setup(nullptr, this);
        }

        return;
    }

    nameLabel->Enable();

    for(int i = 0; i < (int)settlement::ResourceTypes::NONE; ++i)
    {
        auto item = *resourceItems.Get(i);

        auto resource = settlement->GetResource(settlement::ResourceTypes(i));
        if(resource == nullptr)
        {
            item->Disable();
            continue;
        }

        item->Enable();
        item->Setup(resource, settlement);
    }
}