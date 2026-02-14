#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/SimpleList.h"
#include "FlumenEngine/Interface/ProgressBar.h"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/WorldScene.h"

#include "EconomyTab.h"
#include "ResourceItem.h"
#include "JobHoverInfo.h"
#include "ResourceHoverInfo.h"
#include "JobItem.h"
#include "RuleMenu.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.8f;

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

    resourceItems.Initialize((int)settlement::ResourceTypes::NONE);

    resourceItemLayout = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 90.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        },
        resourceItems.GetCapacity(),
        ListOrientations::VERTICAL,
        5.0f
    );
    resourceItemLayout->AddScroller(8, resourceItems.GetCapacity());
    resourceItemLayout->Enable();

    for(auto i = 0; i < resourceItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <ResourceItem>
        (
            {
                Size(600, 35), 
                drawOrder_ + 1, 
                {resourceItemLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *resourceItems.Allocate() = item;
    }

    static const auto MAX_SETTLEMENT_POPULATION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAX_SETTLEMENT_POPULATION).Integer;

    jobItems.Initialize(MAX_SETTLEMENT_POPULATION);

    jobItemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_,
            {Position2{0.0f, -15.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}
        }
    );
    jobItemLayout->SetDistancing(MAX_SETTLEMENT_POPULATION / 2, 4.0f);
    jobItemLayout->AlignToCenter();
    jobItemLayout->Enable();

    for(auto i = 0; i < MAX_SETTLEMENT_POPULATION; ++i)
    {
        auto item = ElementFactory::BuildElement <JobItem>
        (
            {
                Size(40, 48), 
                drawOrder_ + 1, 
                {jobItemLayout}, 
                {"panel-border-015", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *jobItems.Allocate() = item;
    }

    resourceHoverInfo = ElementFactory::BuildElement <ResourceHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );
    resourceHoverInfo->Disable();

    jobHoverInfo = ElementFactory::BuildElement <JobHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );
    jobHoverInfo->Disable();

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &EconomyTab::HandleSettlementChanged};
}

void EconomyTab::HandleUpdate()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    auto settlement = ruleMenu->GetCurrentSettlement();

    if(settlement == nullptr)
        return;

    nameLabel->Setup(settlement->GetName());

    auto text = Phrase() << settlement->GetPopulation() << " " << settlement->GetRace()->PluralName << " live in this jolly settlement, part of the realm of " << settlement->GetRuler()->GetName() << ".";
    populationLabel->Setup(text);

    text = Phrase() << settlement->GetName() << " has " << settlement->GetFreeWorkerCount() << " free workers.";
    workerLabel->Setup(text);

    SetupJobItems();
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
        item->Setup(resource, settlement, this);
    }
}

void EconomyTab::SetupJobItems()
{
    /*for(auto &item : jobItems)
    {
        item->Disable();
    }*/

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    const auto &instructions = polity::HumanMind::Get()->GetSettlementInstructions();

    if(instructions == nullptr)
        return;

    auto workerCount = ruleMenu->GetCurrentSettlement()->GetPopulation();

    auto item = jobItems.GetStart();
    for(int i = 0; i < instructions->GetCapacity(); ++i)
    {
        auto instruction = instructions->Find(i);

        auto isWorkerHired = i < workerCount;

        (*item)->Setup(instruction, this, isWorkerHired);
        //(*item)->Enable();

        item++;
    }
}