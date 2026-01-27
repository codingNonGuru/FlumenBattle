#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/SimpleList.h"
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

    storageBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(80, 28), drawOrder_ + 1, {Position2(190.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    storageBar->Enable();

    storedLabel = ElementFactory::BuildText(
        {drawOrder_ + 3, {Position2(0.0f, 0.0f), storageBar}}, 
        {{"VerySmall"}, Color::WHITE}
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

    auto storageRatio = (float)resource->Storage / (float)settlement->GetStorage();
    storageBar->SetProgress(storageRatio);
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
        resource->Type->Type == settlement::ResourceTypes::CLOTHING || 
        resource->Type->Type == settlement::ResourceTypes::POTTERY ||
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

    itemLayout = ElementFactory::BuildSimpleList
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
    itemLayout->AddScroller(8, resourceItems.GetCapacity());
    itemLayout->Enable();

    for(auto i = 0; i < resourceItems.GetCapacity(); ++i)
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
        item->Setup(resource, settlement);
    }
}