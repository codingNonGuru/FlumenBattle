#include "Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"

using namespace world;

void Polity::Initialize(settlement::Settlement *ruler)
{
    this->ruler = ruler;

    this->settlements.Initialize(64);
    *this->settlements.Add() = ruler;

    malariaDeathCount = 0;
}

void Polity::ExtendRealm(settlement::Settlement *domain)
{
    *this->settlements.Add() = domain;
}

Integer Polity::GetPopulation() const
{
    auto population = 0;
    for(auto settlement : settlements)
    {
        population += settlement->GetPopulation();
    }

    return population;
}

Integer Polity::GetIndustrialPower() const
{
    auto industry = 0;
    for(auto settlement : settlements)
    {
        industry += settlement->GetIndustrialProduction();
    }

    return industry;
}

void Polity::Update() 
{
    malariaDeathCount = 0;

    for(auto settlement : settlements)
    {
        for(auto event : settlement->events)
        {
            if(event.EventType == SettlementEventTypes::DEPOPULATION)
            {
                malariaDeathCount++;
            }    
        }
    }
}