#include "Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace world;

void Polity::Initialize(settlement::Settlement *ruler)
{
    this->ruler = ruler;

    this->settlements.Initialize(256);

    malariaDeathCount = 0;

    technologyRoster = new science::TechnologyRoster();

    auto &worldTiles = WorldScene::Get()->GetTiles();

    interestMap.Initialize(worldTiles.GetWidth(), worldTiles.GetHeight());
    for(int x = 0; x < worldTiles.GetWidth(); ++x)
    {
        for(int y = 0; y < worldTiles.GetHeight(); ++y)
        {
            auto interestTile = interestMap.Get(x, y);
            *interestTile = {worldTiles.Get(x, y)->HexCoordinates, nullptr, 0};
        }   
    }

    //*this->settlements.Add() = ruler;
    ExtendRealm(ruler);
}

void Polity::ExtendRealm(settlement::Settlement *domain)
{
    *this->settlements.Add() = domain;

    auto &tiles = domain->GetLocation()->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE, 2);
    for(auto &tile : tiles)
    {
        auto existingInterest = interestMap.GetTile(tile->HexCoordinates);
        if(tile == domain->GetLocation())
        {
            existingInterest->Owner = domain;
            existingInterest->Distance = 1;
        }
        else
        {
            //auto distance = domain->GetLocation()->GetDistanceTo(*tile);
            auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra(domain->GetLocation(), tile);
            auto cost = pathData.Complexity;

            auto nearbyTiles = tile->GetNearbyTiles(1, 3);
            for(auto neighbour = nearbyTiles.GetStart(); neighbour != nearbyTiles.GetEnd(); ++neighbour)
            {
                if((*neighbour)->HasBiome(WorldBiomes::STEPPE))
                {
                    cost -= 2;
                }
                else if((*neighbour)->HasBiome(WorldBiomes::WOODS))
                {
                    cost -= 1;
                }
            }

            if(existingInterest->Owner == nullptr)
            {
                existingInterest->Owner = domain;
                existingInterest->Distance = cost;
            }
            else
            {
                if(existingInterest->Distance > cost)
                {
                    existingInterest->Owner = domain;
                    existingInterest->Distance = cost;
                }
                else if(existingInterest->Distance == cost)
                {
                    if(utility::RollDice(0.5f) == true)
                    {
                        existingInterest->Owner = domain;
                    }
                }
            }
        }
    }
}

void Polity::DecideResearch()
{
    if(technologyRoster->IsResearchingAnything())
        return;

    struct Priority
    {
        science::Technologies Technology;

        int Level;
    };

    static const container::Array <Priority> priorities = {
        {science::Technologies::MASONRY, 0}, 
        {science::Technologies::HAND_WASHING, 1}, 
        {science::Technologies::TRAINED_SENTINELS, 2}};

    for(int i = 0; i < 3; ++i)
    {
        for(auto priority = priorities.GetStart(); priority != priorities.GetEnd(); ++priority)
        {
            if(priority->Level == i && technologyRoster->HasDiscovered(priority->Technology) == false)
            {
                technologyRoster->StartResearching(priority->Technology);
                
                return;
            }
        }
    }
}

void Polity::ApplyTechnologyModifiers(settlement::Settlement *settlement) const
{
    technologyRoster->ApplyModifiers(*settlement);
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

Integer Polity::GetScientificPower() const
{
    auto science = 0;
    for(auto settlement : settlements)
    {
        science += settlement->GetScienceProduction();
    }

    return science;
}

bool Polity::HasDiscoveredTechnology(science::Technologies technology) const
{
    return technologyRoster->HasDiscovered(technology);
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

    DecideResearch();

    technologyRoster->Update(*this);
}