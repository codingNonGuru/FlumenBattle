#include "Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Faction.h"
#include "FlumenBattle/World/PolityAllocator.h"

using namespace world::polity;

void Polity::Initialize(settlement::Settlement *ruler)
{
    this->ruler = ruler;

    //this->settlements.Initialize(256);

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

    ExtendRealm(ruler);
}

void Polity::ExtendRealm(settlement::Settlement *domain)
{
    *this->settlements.Add() = domain;

    domain->SetPolity(this);

    MapInterest(domain);
}

void Polity::RemoveSettlement(settlement::Settlement *settlement)
{
    this->settlements.Remove(settlement);

    settlement->SetPolity(nullptr);

    auto &interests = interestMap.GetTiles();
    for(auto interest = interests.GetStart(); interest != interests.GetEnd(); ++interest)
    {
        if(interest->Owner == settlement)
        {
            interest->Owner = nullptr;
        }
    }

    for(auto &link : settlement->GetLinks())
    {
        auto other = link.Path->GetOther(settlement);
        if(other->GetPolity() != this)
            continue;

        MapInterest(other);
    }
}

void Polity::UndergoDivision(Faction *faction)
{
    for(auto &member : faction->GetMembers())
    {
        RemoveSettlement(member);

        member->SetFaction(nullptr);
    }
}

void Polity::MapInterest(settlement::Settlement *domain)
{
    auto &mappedTiles = utility::Pathfinder <WorldTile>::Get()->MapArea(domain->GetLocation(), MAXIMUM_COLONIZATION_RANGE);

    for(auto &tile : mappedTiles)
    {
        auto existingInterest = interestMap.GetTile(tile.Tile->HexCoordinates);
        if(tile.Tile == domain->GetLocation())
        {
            existingInterest->Owner = domain;
            existingInterest->Distance = 0;
            existingInterest->Value = 5;
        }
        else
        {
            auto distance = tile.Distance; //pathData.Complexity; 

            auto value = 0;
            auto nearbyTiles = tile.Tile->GetNearbyTiles();
            for(auto neighbour = nearbyTiles.GetStart(); neighbour != nearbyTiles.GetEnd(); ++neighbour)
            {
                if((*neighbour)->HasBiome(WorldBiomes::STEPPE))
                {
                    value += 2;
                }
                else if((*neighbour)->HasBiome(WorldBiomes::WOODS))
                {
                    value += 1;
                }
            }

            existingInterest->Value = value;

            if(existingInterest->Owner == nullptr)
            {
                existingInterest->Owner = domain;
                existingInterest->Distance = distance;
            }
            else
            {
                if(existingInterest->Distance > distance)
                {
                    existingInterest->Owner = domain;
                    existingInterest->Distance = distance;
                }
                else if(existingInterest->Distance == distance)
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

Faction *Polity::FindFaction(settlement::Settlement *settlement)
{
    bool hasFound = false;
    for(auto &faction : factions)
    {
        for(auto &member : faction.GetMembers())
        {
            if(settlement->GetPathTo(member) != nullptr)
            {
                faction.AddMember(settlement);
                hasFound = true;
                return &faction;
            }
        }
    }

    if(hasFound == false)
    {
        auto faction = PolityAllocator::Get()->AllocateFaction(this);
        faction->Initialize(this);
        faction->AddMember(settlement);
        faction->SetLeader(settlement);
        return faction;
    }

    return nullptr;
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

static container::Array <FactionDecision> decisions = container::Array <FactionDecision> (8);

container::Array <FactionDecision> &Polity::Update() 
{
    malariaDeathCount = 0;

    for(auto settlement : settlements)
    {
        for(auto event : settlement->events)
        {
            if(event.EventType == settlement::EventTypes::DEPOPULATION)
            {
                malariaDeathCount++;
            }    
        }
    }

    DecideResearch();

    technologyRoster->Update(*this);

    
    decisions.Reset();
    for(auto &faction : factions)
    {
        auto decision = faction.Update();

        if(decision.Decision == FactionDecisions::DECLARE_INDEPENDENCE)
        {
            *decisions.Add() = decision;
        }
    }

    return decisions;
}