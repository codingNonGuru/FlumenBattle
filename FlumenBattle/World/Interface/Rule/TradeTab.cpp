#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "TradeTab.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Shipment.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Settlement/TradeHandler.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_ITEM_COUNT 128

#define VISIBLE_ITEM_COUNT 8

#define ITEM_LIFETIME_IN_HOURS 72

#define VISIBLE_LINK_ITEM_COUNT 4

auto recordedShipments = container::Pool <world::settlement::Shipment> (MAXIMUM_ITEM_COUNT);

void LinkItem::HandleConfigure()
{
    settlementLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(5.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    settlementLabel->Enable();

    relationshipBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(60, 24), drawOrder_ + 1, {Position2(240.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    relationshipBar->Enable();

    levelLabel = ElementFactory::BuildText(
        {drawOrder_ + 3, {relationshipBar}}, 
        {{"VerySmall"}, Color::WHITE}
    );
    levelLabel->Enable();
}

void LinkItem::Setup(const settlement::Link *link)
{
    settlementLabel->Setup(link->Other->GetName());

    auto progress = settlement::TradeHandler::GetNextLevelProgress(link->Traffic);
    relationshipBar->SetProgress(progress);

    auto level = settlement::TradeHandler::GetRelationshipLevel(link->Traffic);
    levelLabel->Setup(ShortWord() << level);
}

void TradeItem::HandleConfigure()
{
    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(220.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, 
            {"Radish", false}
        }
    );
    icon->Enable();

    settlementLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(5.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    settlementLabel->Enable();

    volumeLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(300.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    volumeLabel->Enable();

    timeLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(380.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"VerySmall"}, TEXT_COLOR}
    );
    timeLabel->Enable();
}

void TradeItem::HandleUpdate()
{
    auto hourCount = world::WorldScene::Get()->GetTime().GetTickCount() - shipment->TimeInAbsoluteTicks;
    hourCount /= WorldTime::TICKS_PER_HOUR;

    if(hourCount < WorldTime::HOURS_IN_DAY)
    {
        timeLabel->Setup(Word() << hourCount << " hrs");
    }
    else
    {
        timeLabel->Setup(Word() << hourCount / WorldTime::HOURS_IN_DAY << " days");
    }
}

void TradeItem::Setup(const settlement::Shipment &shipment, settlement::Settlement *settlement)
{
    this->shipment = &shipment;

    auto textureName = settlement::ResourceFactory::Get()->CreateType(shipment.Resource)->TextureName;
    icon->SetTexture(textureName);

    if(shipment.To == settlement)
    {
        settlementLabel->Setup(Word() << "from " << shipment.From->GetName());
    }
    else
    {
        settlementLabel->Setup(Word() << "to " << shipment.To->GetName());
    }

    volumeLabel->Setup(ShortWord() << shipment.AmountReceived << "/" << shipment.AmountSent);
}

void TradeTab::HandleConfigure()
{
    tradeItemList = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_ + 1,
            {Position2{0.0f, 40.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {"panel-border-015", true},
            Opacity(0.7f)
        },
        MAXIMUM_ITEM_COUNT,
        0.0f
    );
    tradeItemList->SetSpriteColor(BORDER_COLOR);
    tradeItemList->MakeScrollable(VISIBLE_ITEM_COUNT, MAXIMUM_ITEM_COUNT);
    tradeItemList->Enable();

    tradeItems.Initialize(MAXIMUM_ITEM_COUNT);
    for(int i = 0; i < tradeItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <TradeItem>
        (
            {
                Size(500, 30), 
                drawOrder_ + 1, 
                {tradeItemList}, 
                {false},
                Opacity(0.0f)
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *tradeItems.Allocate() = item;
    }

    static const auto MAXIMUM_PATHS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_PATHS_PER_SETTLEMENT).Integer;

    linkItemList = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_,
            {Position2{0.0f, -10.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        },
        MAXIMUM_PATHS_PER_SETTLEMENT,
        5.0f
    );
    linkItemList->MakeScrollable(VISIBLE_LINK_ITEM_COUNT, MAXIMUM_PATHS_PER_SETTLEMENT);
    linkItemList->Enable();

    linkItems.Initialize(MAXIMUM_PATHS_PER_SETTLEMENT);
    for(int i = 0; i < linkItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <LinkItem>
        (
            {
                Size(400, 35), 
                drawOrder_ + 1, 
                {linkItemList}, 
                {"panel-border-015", true},
                Opacity(0.7f)
            }
        );
        item->SetSpriteColor(BORDER_COLOR);

        *linkItems.Allocate() = item;
    }

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &TradeTab::HandleSettlementChanged};

    polity::HumanMind::Get()->OnTradeShipment += {this, &TradeTab::HandlePlayerShipment};
}

void TradeTab::HandleUpdate()
{
    if(settlement == nullptr)
        return;

    for(auto &shipment : recordedShipments)
    {
        auto tickCount = world::WorldScene::Get()->GetTime().GetTickCount() - shipment.TimeInAbsoluteTicks;

        if(tickCount >= ITEM_LIFETIME_IN_HOURS * world::WorldTime::TICKS_PER_HOUR)
        {
            recordedShipments.RemoveAt(&shipment);
        }
    }

    tradeItemList->SetScrollableChildCount(recordedShipments.GetSize());

    for(auto &item : tradeItems)
    {
        item->Disable();
    }

    auto item = tradeItems.GetStart();
    for(auto &shipment : recordedShipments)
    {
        (*item)->Setup(shipment, settlement);

        (*item)->Enable();

        item++;
    }

    linkItemList->SetScrollableChildCount(settlement->GetLinks().GetSize());

    for(auto &linkItem : linkItems)
    {
        linkItem->Disable();
    }

    auto linkItem = linkItems.GetStart();
    for(auto &link : settlement->GetLinks())
    {
        if(link.Traffic == 0)
        {
            continue;
        }

        (*linkItem)->Setup(&link);

        (*linkItem)->Enable();

        linkItem++;
    }     
}

void TradeTab::HandlePlayerShipment()
{
    if(settlement == nullptr)
        return;

    auto shipment = polity::HumanMind::Get()->GetCurrentShipment();
    if(shipment.From != settlement && shipment.To != settlement)
        return;

    if(recordedShipments.IsFull() == true)
        return;

    *recordedShipments.Add() = shipment;
}

void TradeTab::HandleSettlementChanged()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        tradeItemList->Disable();

        linkItemList->Disable();

        recordedShipments.Reset();

        return;
    }

    tradeItemList->Enable();

    linkItemList->Enable();
}