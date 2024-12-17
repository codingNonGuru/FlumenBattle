#include "TradeHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::settlement;

#define TIME_BETWEEN_SHIPMENTS 12

#define VOLUME_PER_SHIPMENT 20

#define SHIPMENT_VOLUME_LOSS 6

auto priorities = container::Array <ShipmentPriority> (256);

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

    const auto timeModifier = settlement.GetModifier(Modifiers::DURATION_BETWEEN_TRADE_SHIPMENTS);

    lastShipmentTime = TIME_BETWEEN_SHIPMENTS + timeModifier;

    finalPriority = *highestPriority;

    willShipThisTurn = true;
}

#define RELATIONSHIP_GAIN_PER_SHIPMENT 100

#define RELATIONSHIP_LIMIT 1000

#define RELATIONSHIP_DECAY_RATE 1

void TradeHandler::SendTransport(Settlement &settlement)
{
    if(willShipThisTurn == false)
        return;

    int volumeSent = VOLUME_PER_SHIPMENT;
    settlement.GetResource(finalPriority.Resource)->Storage -= volumeSent;

    int volumeReceived = VOLUME_PER_SHIPMENT - SHIPMENT_VOLUME_LOSS;
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
        else if(link.Traffic > RELATIONSHIP_LIMIT)
        {
            link.Traffic = RELATIONSHIP_LIMIT;
        }
    }
}