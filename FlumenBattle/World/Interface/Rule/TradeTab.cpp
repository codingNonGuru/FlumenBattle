#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "TradeTab.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Shipment.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_ITEM_COUNT 32

#define ITEM_LIFETIME_IN_HOURS 72

auto recordedShipments = container::Pool <world::settlement::Shipment> (MAXIMUM_ITEM_COUNT);

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
    auto hourCount = world::WorldScene::Get()->GetTime().TotalHourCount - shipment->TimeInAbsoluteTicks;

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
    itemList = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_,
            {Position2{0.0f, 40.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        },
        MAXIMUM_ITEM_COUNT,
        5.0f
    );
    itemList->Enable();

    items.Initialize(MAXIMUM_ITEM_COUNT);
    for(int i = 0; i < items.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <TradeItem>
        (
            {
                Size(500, 35), 
                drawOrder_ + 1, 
                {itemList}, 
                {"panel-border-015", true},
                Opacity(0.7f)
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *items.Allocate() = item;
    }

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &TradeTab::HandleSettlementChanged};

    polity::HumanMind::Get()->OnTradeShipment += {this, &TradeTab::HandlePlayerShipment};
}

void TradeTab::HandleUpdate()
{
    if(settlement == nullptr)
        return;

    for(auto &item : items)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto &shipment : recordedShipments)
    {
        (*item)->Setup(shipment, settlement);

        (*item)->Enable();

        item++;
    }    
}

void TradeTab::HandlePlayerShipment()
{
    if(settlement == nullptr)
        return;

    auto shipment = polity::HumanMind::Get()->GetCurrentShipment();
    if(shipment.From != settlement && shipment.To != settlement)
        return;

    *recordedShipments.Add() = shipment;

    for(auto &shipment : recordedShipments)
    {
        auto tickCount = world::WorldScene::Get()->GetTime().TotalHourCount - shipment.TimeInAbsoluteTicks;

        if(tickCount >= ITEM_LIFETIME_IN_HOURS)
        {
            recordedShipments.RemoveAt(&shipment);
        }
    }
}

void TradeTab::HandleSettlementChanged()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        itemList->Disable();

        return;
    }

    recordedShipments.Reset();

    itemList->Enable();
}