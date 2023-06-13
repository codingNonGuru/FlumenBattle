#include "SettlementEvent.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::settlement;

void SettlementEventGenerator::GenerateEvent(Settlement &settlement, const AfflictionResult &result)
{
    if(result.Type == AfflictionResultTypes::DEATH)
    {
        *settlement.events.Add() = SettlementEvent(SettlementEventTypes::DEPOPULATION, result.AfflictionType);
    }
}