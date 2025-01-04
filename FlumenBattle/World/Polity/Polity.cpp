#include "Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/World/Polity/PolityAllocator.h"
#include "FlumenBattle/World/Polity/Mind.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/MachineMind.h"

using namespace world::polity;

void Polity::Initialize(settlement::Settlement *ruler, bool isPlayerControlled)
{
    isValid = true;

    if(isPlayerControlled == true)
    {
        controller = HumanMind::Get();
    }
    else
    {
        controller = MachineMind::Get();
    }

    this->ruler = ruler;

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

    ruler->UpdateColonialMap();
}

void Polity::ExtendRealm(settlement::Settlement *domain)
{
    *settlements.Add() = domain;

    settlements;

    domain->SetPolity(this);

    MapInterest(domain);
}

static auto neighboursToRemoved = container::Array <world::settlement::Settlement *> (256);

const container::Array <world::settlement::Settlement *> &Polity::GetSecederNeighbours() const
{
    return neighboursToRemoved;
}

void Polity::RemoveSettlement(settlement::Settlement *settlement)
{
    this->settlements.Remove(settlement);

    settlement->SetPolity(nullptr);

    //Remove interest from interest map of polity
    auto &interests = interestMap.GetTiles();
    for(auto interest = interests.GetStart(); interest != interests.GetEnd(); ++interest)
    {
        if(interest->Owner == settlement)
        {
            interest->Owner = nullptr;
        }
    }

    //Find all the settlements neighbor to settlement that is to be removed that share the same polity
    for(auto &link : settlement->GetLinks())
    {
        auto other = link.Path->GetOther(settlement);
        if(other->GetPolity() != this)
            continue;

        bool hasFound = false;
        for(auto &neighbour : neighboursToRemoved)
        {
            if(neighbour == other)
            {
                hasFound = true;
                break;
            }
        }

        if(hasFound == false)
        {
            *neighboursToRemoved.Add() = other;
        }
    }
}

void Polity::UndergoDivision(Faction *faction)
{
    neighboursToRemoved.Reset();

    for(auto &member : faction->GetMembers())
    {
        RemoveSettlement(member);

        member->SetFaction(nullptr);
    }

    for(auto &neighbour : neighboursToRemoved)
    {
        MapInterest(neighbour);
    }
}

void Polity::UndergoDivision(settlement::Settlement *settlement)
{
    neighboursToRemoved.Reset();

    RemoveSettlement(settlement);

    if(settlement->GetFaction() != nullptr)
    {
        settlement->GetFaction()->RemoveMember(settlement);

        settlement->SetFaction(nullptr);
    }

    for(auto &neighbour : neighboursToRemoved)
    {
        MapInterest(neighbour);
    }
}

void Polity::MapInterest(settlement::Settlement *domain)
{
    auto &mappedTiles = utility::Pathfinder <tile::WorldTile>::Get()->MapArea(domain->GetLocation(), MAXIMUM_COLONIZATION_RANGE);

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
        if(faction.IsActive() == false)
            continue;

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
    controller->DecideResearch(*this);
}

void Polity::ApplyTechnologyModifiers(settlement::Settlement *settlement) const
{
    technologyRoster->ApplyModifiers(*settlement);
}

void Polity::ProcessTrade()
{
    controller->ProcessTrade(*this);
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

void Polity::AddTechnology(science::Technologies technology)
{
    if(technologyRoster->HasDiscovered(technology) == false)
    {
        technologyRoster->AddTechnology(technology);
    }
}

const world::science::TechnologyType *Polity::GetResearchTarget() const
{
    return technologyRoster->GetTarget();
}

int Polity::GetResearchProgress() const
{
    return technologyRoster->GetProgress();
}

static container::Array <FactionDecision> decisions = container::Array <FactionDecision> (8);

container::Array <FactionDecision> &Polity::Update() 
{
    decisions.Reset();

    if(isValid == false)
    {
        turnsUntilDeletion--;
        return decisions;
    }

    malariaDeathCount = 0;

    for(auto &settlement : settlements)
    {
        for(auto event : settlement->events)
        {
            if(event.EventType == settlement::EventTypes::DEPOPULATION)
            {
                malariaDeathCount++;
            }    
        }
    }

    technologyRoster->Update(*this);

    auto fusionData = CheckFactionMergers();
    if(fusionData.First != nullptr)
    {
        MergeFactions(fusionData);

        return decisions;
    }

    for(auto &faction : factions)
    {
        if(faction.IsActive() == false)
            continue;

        auto decision = faction.Update();

        if(decision.Decision == FactionDecisions::DECLARE_INDEPENDENCE)
        {
            *decisions.Add() = decision;
        }
    }

    return decisions;
}

void Polity::Decide()
{
    if(isValid == false)
        return;

    DecideResearch();

    controller->DecideBorders(*this);

    controller->MakeDecision(*this);

    controller->ProcessTrade(*this);
}

Polity::FusionData Polity::CheckFactionMergers()
{
    for(auto &faction : factions)
    {
        if(faction.IsActive() == false)
            continue;

        for(auto &member : faction.GetMembers())
        {
            for(auto link : member->GetLinks())
            {
                auto other = link.Path->GetOther(member);

                if(other->GetFaction() != nullptr && other->GetFaction() != &faction && other->GetPolity() == this)
                {
                    return {&faction, other->GetFaction()};
                }
            }
        }
    }

    return {};
}

void Polity::MergeFactions(Polity::FusionData fusionData)
{
    for(auto &member : fusionData.Second->GetMembers())
    {
        fusionData.First->AddMember(member);
        member->SetFaction(fusionData.First);
    }

    PolityAllocator::Get()->FreeFaction(this, fusionData.Second);
}

bool Polity::IsPlayerControlled() const
{
    return controller == HumanMind::Get();
}

void Polity::SetController(bool isPlayer)
{
    if(isPlayer == true)
    {
        controller = HumanMind::Get();
    }
    else
    {
        controller = MachineMind::Get();
    }
}

void Polity::CleanUp()
{
    for(auto &faction : factions)
    {
        if(faction.GetMembers().GetSize() > 0)
            continue;

        polity::PolityAllocator::Get()->FreeFaction(this, &faction);   
    }

    for(auto &settlement : settlements)
    {
        if(settlement->IsValid() == true)
            continue;

        UndergoDivision(settlement);
    }

    if(settlements.GetSize() == 0 && isValid == true)
    {
        MarkForDeletion();
    }
}

void Polity::MarkForDeletion()
{
    isValid = false;

    turnsUntilDeletion = 3;

    controller->RegisterMarkForDeletion();
}

void Polity::RegisterPopIncrease(settlement::Settlement *settlement)
{
    controller->RegisterPopIncrease(settlement);
}

void Polity::RegisterTileExplored(settlement::Settlement *settlement, tile::WorldTile *tile)
{
    controller->RegisterTileExplored(settlement, tile);
}

void Polity::RecalculateLinks()
{
    neighborHandler.CalculateNeighbors(this);
}