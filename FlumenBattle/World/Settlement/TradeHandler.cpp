#include "TradeHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::settlement;

#define TIME_BETWEEN_SHIPMENTS 12

#define VOLUME_PER_SHIPMENT 20

auto priorities = container::Array <ShipmentPriority> (256);

const auto RELATIVE_THRESHOLDS = container::Array <int> {0, 1000, 2000, 3000, 5000, 7000};

static const auto &ABSOLUTE_THRESHOLDS = [&RELATIVE_THRESHOLDS]
{
    static container::Array <int> thresholds (RELATIVE_THRESHOLDS.GetSize());

    int amount = 0;
    for(auto &threshold : RELATIVE_THRESHOLDS)
    {
        amount += threshold;

        *thresholds.Add() = amount;
    }

    return thresholds;
} ();

static const auto MAXIMUM_TRAFFIC = [&RELATIVE_THRESHOLDS] 
{
    int amount = 0;
    for(auto &threshold : RELATIVE_THRESHOLDS)
    {
        amount += threshold;
    }

    return amount;
} ();

void TradeHandler::PrepareTransport(Settlement &settlement)
{
    willShipThisTurn = false;

    lastShipmentTime--;

    if(lastShipmentTime < 0)
    {
        lastShipmentTime = 0;
    }

    if(lastShipmentTime > 0)
        return;

    priorities.Reset();

    for(int i = 0; i < (int)ResourceTypes::NONE; ++i)
    {
        auto resource = settlement.GetResource(ResourceTypes(i));

        if(resource == nullptr)
            continue;

        if(resource->AbundanceDegree == 0)
            continue;

        for(auto &link : settlement.GetLinks())
        {
            auto otherResource = link.Other->GetResource(ResourceTypes(i));

            if(otherResource->ScarcityDegree == 0)
                continue;

            auto priority = resource->AbundanceDegree + otherResource->ScarcityDegree;

            //priority += link.Path->Complexity < 5 ? 1 : 0;

            *priorities.Add() = {link.Other, priority, ResourceTypes(i)};
        }
    }

    ShipmentPriority *highestPriority = nullptr;
    for(auto &priority : priorities)
    {
        if(highestPriority == nullptr)
        {
            highestPriority = &priority;
        }
        else if(priority.Degree > highestPriority->Degree)
        {
            highestPriority = &priority;
        }
    }

    if(highestPriority == nullptr)
        return;

    lastShipmentTime = GetTimeBetweenShipments(settlement);

    finalPriority = *highestPriority;

    willShipThisTurn = true;
}

#define RELATIONSHIP_GAIN_PER_SHIPMENT 200

#define RELATIONSHIP_DECAY_RATE 1

void TradeHandler::SendTransport(Settlement &settlement)
{
    if(willShipThisTurn == false)
        return;

    int volumeSent = VOLUME_PER_SHIPMENT;
    settlement.GetResource(finalPriority.Resource)->Storage -= volumeSent;

    auto volumeLoss = [&] 
    {
        auto toLink = settlement.GetLinks().Find(finalPriority.To);
        auto level = GetRelationshipLevel(toLink->Traffic);

        if(level == 1)
            return 2 + utility::RollD8Dice();
        else if(level < 4)
            return 1 + utility::RollD6Dice();
        else 
            return utility::RollD4Dice();
    } ();

    int volumeReceived = VOLUME_PER_SHIPMENT - volumeLoss;
    finalPriority.To->ReceiveTransport(finalPriority.Resource, volumeReceived);

    willShipThisTurn = false;

    lastOutgoingShipment = {&settlement, finalPriority.To, finalPriority.Resource, volumeSent, volumeReceived, world::WorldScene::Get()->GetTime().GetTickCount()};

    auto link = settlement.GetLinks().Find(finalPriority.To);
    link->Traffic += RELATIONSHIP_GAIN_PER_SHIPMENT;

    link = finalPriority.To->GetLinks().Find(&settlement);
    link->Traffic += RELATIONSHIP_GAIN_PER_SHIPMENT;
}

void TradeHandler::FinishUpdate(Settlement &settlement)
{
    for(auto &link : settlement.GetLinks())
    {
        link.Traffic -= RELATIONSHIP_DECAY_RATE;

        if(link.Traffic < 0)
        {
            link.Traffic = 0;
        }
        else if(link.Traffic > MAXIMUM_TRAFFIC)
        {
            link.Traffic = MAXIMUM_TRAFFIC;
        }
    }
}

int TradeHandler::GetTimeBetweenShipments(const Settlement &settlement) const
{
    const auto timeModifier = settlement.GetModifier(Modifiers::DURATION_BETWEEN_TRADE_SHIPMENTS);

    return TIME_BETWEEN_SHIPMENTS + timeModifier;
}

float TradeHandler::GetProgress(const Settlement &settlement) const
{
    return 1.0f - (float)lastShipmentTime / (float)GetTimeBetweenShipments(settlement);
}

int TradeHandler::GetRelationshipLevel(int traffic)
{
    if(traffic == MAXIMUM_TRAFFIC)
        return ABSOLUTE_THRESHOLDS.GetSize() - 1;

    for(int i = 1; i < ABSOLUTE_THRESHOLDS.GetSize(); ++i)
    {
        auto previous = *ABSOLUTE_THRESHOLDS.Get(i - 1);

        auto current = *ABSOLUTE_THRESHOLDS.Get(i);

        if(traffic >= previous && traffic < current)
            return i;
    }
}

float TradeHandler::GetNextLevelProgress(int traffic)
{
    if(traffic == MAXIMUM_TRAFFIC)
        return 1.0f;

    for(int i = 1; i < ABSOLUTE_THRESHOLDS.GetSize(); ++i)
    {
        auto previous = *ABSOLUTE_THRESHOLDS.Get(i - 1);

        auto current = *ABSOLUTE_THRESHOLDS.Get(i);

        if(traffic >= previous && traffic < current)
        {
            auto delta = *RELATIVE_THRESHOLDS.Get(i);

            return float(traffic - previous) / float(delta);
        }
    }
}

int TradeHandler::GetMaximumTraffic()
{
    return MAXIMUM_TRAFFIC;
}