#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "BuildingTab.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/Rule/BuildingHoverInfo.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Interface/Rule/ProductionDecisionInterface.h"
#include "FlumenBattle/World/Interface/Rule/ProductionQueueItem.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_BUILDING_COUNT 16

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

    //ruleMenu->GetCurrentSettlement()->HireWorker(building);

    polity::HumanMind::Get()->ChangeBuildingWorkforce(building, true);
}

void BuildingItem::HandleRightClick()
{
    if(building == nullptr)
        return;

    //ruleMenu->GetCurrentSettlement()->FireWorker(building);

    polity::HumanMind::Get()->ChangeBuildingWorkforce(building, false);
}

void BuildingItem::Setup(settlement::Building *building, BuildingTab *tab)
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

void BuildingTab::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    decisionInterface = ElementFactory::BuildElement <interface::rule::ProductionDecisionInterface>
    (
        {
            Size(320, 400), 
            drawOrder_, 
            {Position2(0.0f, 30.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        new ProductionInterfaceData{settlement::ProductionClasses::BUILDING}
    );
    decisionInterface->Enable();

    buildingLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_,
            {Position2{0.0f, -10.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}
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
                drawOrder_ + 2, 
                {buildingLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *buildingItems.Allocate() = item;
    }

    static const auto PRODUCTION_QUEUE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::PRODUCTION_QUEUE_SIZE).Integer;

    queueItems.Initialize(PRODUCTION_QUEUE_SIZE);

    queueItemList = ElementFactory::BuildSimpleList <SimpleList>
    (
        { 
            drawOrder_,
            {Position2{0.0f, -5.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::LOWER_CENTER, nullptr}
        },
        PRODUCTION_QUEUE_SIZE,
        ListOrientations::HORIZONTAL,
        5.0f
    );
    queueItemList->SetFixedElementCount(PRODUCTION_QUEUE_SIZE);
    queueItemList->Enable();

    queueItemList->SetDynamicParent(buildingLayout);

    for(auto i = 0; i < PRODUCTION_QUEUE_SIZE; ++i)
    {
        auto item = ElementFactory::BuildElement <ProductionQueueItem>
        (
            {
                Size(40, 40), 
                drawOrder_ + 1, 
                {queueItemList}, 
                {"panel-border-015", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *queueItems.Allocate() = item;
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

    ruleMenu->OnSettlementChanged += {this, &BuildingTab::HandleSettlementChanged};
}

void BuildingTab::HandleUpdate()
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

    auto &buildingQueue = polity::HumanMind::Get()->GetBuildingQueue();

    for(auto &queueItem : queueItems)
    {
        queueItem->Disable();
    }

    auto queueItem = queueItems.GetStart();
    for(int priority = 1; priority <= buildingQueue.GetSize(); ++priority)
    {
        auto queueSlot = buildingQueue.Find(priority);

        (*queueItem)->Setup(queueSlot);

        (*queueItem)->Enable();

        queueItem++;
    }

    nameLabel->Setup(settlement->GetName());
}

void BuildingTab::HandleSettlementChanged() 
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        nameLabel->Disable();

        decisionInterface->Disable();

        for(auto &item : buildingItems)
        {
            item->Setup(nullptr, this);
        }

        return;
    }

    decisionInterface->Setup(settlement);

    decisionInterface->Enable();

    nameLabel->Enable();
}