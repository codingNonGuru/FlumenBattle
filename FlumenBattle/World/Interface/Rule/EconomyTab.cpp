#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "EconomyTab.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/Rule/BuildingHoverInfo.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/Race.h"

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
        {drawOrder_ + 1, {Position2(160.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "100"}
    );
    storedLabel->Enable();

    outputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(230.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "+1"}
    );
    outputLabel->Enable();

    inputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(300.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "+1"}
    );
    inputLabel->Enable();
}

void ResourceItem::HandleUpdate()
{
    auto text = Word() << resource->Storage;

    storedLabel->Setup(text);

    text = Word("+") << resource->Production;

    outputLabel->Setup(text);

    text = Word() << resource->Order;

    inputLabel->Setup(text);
}

void ResourceItem::Setup(const settlement::Resource *resource)
{
    this->resource = resource;

    nameLabel->Setup(resource->Type->Name);

    icon->SetTexture(resource->Type->TextureName);
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
                Size(400, 35), 
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
        item->Setup(resource);
    }
}