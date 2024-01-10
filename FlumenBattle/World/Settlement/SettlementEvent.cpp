#include "SettlementEvent.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::settlement;

#define EVENT_EXPIRATION_TIME 120

void EventGenerator::GenerateEvent(Settlement &settlement, const AfflictionResult &result)
{
    auto worldTime = WorldScene::Get()->GetTime().TotalHourCount;

    if(settlement.events.IsFull() == true)
    {
        for(auto &event : settlement.events)
        {
            auto elapsed = worldTime - event.Time;
            if(elapsed > EVENT_EXPIRATION_TIME)
            {
                settlement.events.RemoveAt(&event);
            }
        }
    }

    assert(settlement.events.IsFull() == false && "Settlement Event buffer of Settlement is full.\n");

    if(result.Type == AfflictionResultTypes::DEATH)
    {
        *settlement.events.Add() = Event(EventTypes::DEPOPULATION, result.AfflictionType, worldTime);
    }
}