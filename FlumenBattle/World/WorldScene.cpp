#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/WorldTileModel.h"
#include "FlumenBattle/World/Disaster/Earthquake.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/SettlementFactory.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Faction.h"
#include "FlumenBattle/World/PolityAllocator.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/SimulationMap.h"

#define AWAIT(length) \
    static float timer = 0.0f;\
    timer += Time::GetDelta();\
    if(timer < length)\
        return;\
    else\
        timer = 0.0f;\

namespace world
{
    WorldScene::WorldScene()
    {
        OnUpdateStarted = new Delegate();

        OnPlayerEncounterInitiated = new Delegate();

        OnPlayerEncounterFinished = new Delegate();

        OnPlayerBattleStarted = new Delegate();

        OnPlayerBattleEnded = new Delegate();

        OnSettlementFounded = new Delegate();
    }

    void WorldScene::Initialize()
    {
        time = WorldTime(230, 63, 14);

        battle::BattleScene::Get()->OnRoundEnded += {this, &WorldScene::HandleBattleRoundEnded};

        WorldInterface::Get()->Initialize();        
    }

    void WorldScene::SpeedUpTime()
    {
        time.SlowDown();
    }

    void WorldScene::SlowDownTime()
    {
        time.SpeedUp();
    }

    void WorldScene::Update() 
    {
        OnUpdateStarted->Invoke();

        if(!time)
            return;

        AWAIT(time.GetStep())

        auto refreshCount = time.FlowSpeed == 5 ? 2 : 1;
        for(int i = 0; i < refreshCount; ++i)
        {
            Refresh();
        }
    }

    static auto factionDecisions = container::Array <polity::FactionDecision> (64);

    void WorldScene::Refresh()
    {
        time++;

        auto refreshBattles = [this] 
        {
            static auto finishedBattles = Array <group::Encounter *> (battles->GetCapacity());

            for(auto &battle : *battles)
            {
                battle.Update();

                if(!battle.IsOngoing())
                {
                    *finishedBattles.Allocate() = &battle;
                }
            }

            for(auto &battle : finishedBattles)
            {
                battles->RemoveAt(battle);
            }

            finishedBattles.Reset();
        };

        refreshBattles();

        auto refreshGroups = [this] 
        {
            auto startClock = high_resolution_clock::now();
            
            group::GroupAllocator::Get()->PerformCleanup();

            for(auto &group : *groups)
            {
                group.Update();
            }

            for(auto &group : *groups)
            {
                group.DetermineAction();
            }

            for(auto &group : *groups)
            {
                group.PerformAction();
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - startClock);
            //std::cout <<"group refresh duration " << duration.count() << "\n";
        };

        refreshGroups();

        auto refreshSettlements = [this]
        {
            //if(time.MinuteCount != 0)
                //return;

            auto startClock = high_resolution_clock::now();

            for(auto &settlement : *settlements)
            {
                settlement.Update();
            }

            for(auto &settlement : *settlements)
            {
                settlement.PrepareTransport();
            }

            for(auto &settlement : *settlements)
            {
                settlement.SendTransport();
            }

            for(auto &settlement : *settlements)
            {
                settlement.UpdatePolitics();
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - startClock);
            //std::cout <<"settlement refresh duration " << duration.count() << "\n";
        };

        refreshSettlements();

        if(time.IsNewDay == true)
        {
            auto earthquakeCount = 10;
            for(int i = 0; i < earthquakeCount; ++i)
            {
                disaster::EarthquakeGenerator::Get()->Generate();
            }
        }

        auto refreshPolities = [this]
        {
            auto startClock = high_resolution_clock::now();

            factionDecisions.Reset();
            for(auto &polity : *polities)
            {
                auto &decisions = polity.Update();

                for(auto decision : decisions)
                {
                    *factionDecisions.Add() = decision;
                }
            }

            for(auto &decision : factionDecisions)
            {
                if(decision.Decision == polity::FactionDecisions::DECLARE_INDEPENDENCE)
                {
                    SplitPolity(decision.Faction);
                }
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - startClock);
            //std::cout <<"polity refresh duration " << duration.count() << "\n";
        };

        refreshPolities();

        SimulationMap::Get()->Update();
    }

    void WorldScene::InitiateEncounter(group::Group *first, group::Group *second)
    {
        auto battle = battles->Add();
        battle->Initialize(first, second);

        if(playerGroup == first || playerGroup == second)
        {
            OnPlayerEncounterInitiated->Invoke();

            this->StopTime(1);
        }
    }

    void WorldScene::InitiatePlayerPersuasion()
    {
        playerGroup->SelectAction(group::GroupActions::PERSUADE);

        this->StartTime(1);
    }

    void WorldScene::InitiatePlayerBattle()
    {
        playerGroup->SelectAction(group::GroupActions::FIGHT);

        playerGroup->GetEncounter()->StartFighting();

        OnPlayerBattleStarted->Invoke();

        this->StartTime(1);
    }

    void WorldScene::FinishPlayerBattle()
    {
        playerGroup->SelectAction(group::GroupActions::ENGAGE, {true});

        playerGroup->GetEncounter()->EndFighting();

        OnPlayerBattleEnded->Invoke();
    }

    void WorldScene::FinishPlayerEncounter()
    {
        playerGroup->SelectAction(group::GroupActions::DISENGAGE);

        playerGroup->GetEncounter()->Finish();

        OnPlayerEncounterFinished->Invoke();

        this->StartTime(1);
    }

    static settlement::Settlement *foundedSettlement = nullptr;

    settlement::Settlement * WorldScene::FoundSettlement(WorldTile *location, settlement::Settlement *mother)
    {
        auto settlement = settlement::SettlementFactory::Create({"Safehaven", location});

        auto polity = mother != nullptr ? mother->GetPolity() : nullptr;
        if(polity == nullptr)
        {
            FoundPolity(settlement);
        }
        else
        {
            polity->ExtendRealm(settlement);
        }

        if(mother != nullptr)
        {
            ForgePath(settlement, mother);
        }

        auto &tiles = location->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE, 3);
        for(auto &tile : tiles)
        {
            if(tile == location)
                continue;

            auto other = tile->GetSettlement();
            if(other == nullptr)
                continue;

            if(settlement->GetPathTo(other) != nullptr)
                continue;

            ForgePath(settlement, other, MAXIMUM_COLONIZATION_RANGE);                        
        }

        settlement->UpdateColonialMap();
        settlement->UpdateDistanceToCapital();

        auto &links = settlement->GetLinks();
        for(auto &link : links)
        {
            auto other = link.Other;
            if(other->GetPolity() == polity)
            {
                other->UpdateColonialMap();
                other->UpdateDistanceToCapital();
            }
        }

        foundedSettlement = settlement;
        OnSettlementFounded->Invoke();
    }

    settlement::Settlement *WorldScene::ForgePath(settlement::Settlement *from, settlement::Settlement *to, int complexityLimit)
    {
        auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra(from->GetLocation(), to->GetLocation(), MAXIMUM_COLONIZATION_RANGE - 2);
        
        if(pathData.Complexity > complexityLimit)
            return nullptr;

        auto &tileDatas = pathData.Tiles; 
        if(tileDatas.GetSize() > 0)
        {
            auto path = settlement::SettlementAllocator::Get()->AllocatePath(from, to);
            *path = {from, to};

            for(auto &data : tileDatas)
            {
                path->AddTile(data.Tile);
            }

            from->AddPath(path);
            to->AddPath(path);

            path->Complexity = pathData.Complexity;
        }
    }

    polity::Polity *WorldScene::FoundPolity(settlement::Settlement *ruler)
    {
        auto polity = polity::PolityAllocator::Get()->AllocatePolity();
        polity->Initialize(ruler);

        return polity;
    }

    polity::Polity *WorldScene::SplitPolity(polity::Faction *faction)
    {
        auto polity = faction->GetPolity();

        polity->UndergoDivision(faction);

        auto newPolity = FoundPolity(faction->GetLeader());
        for(auto &member : faction->GetMembers())
        {
            if(member == faction->GetLeader())
                continue;
                
            newPolity->ExtendRealm(member);
        }

        for(auto &member : faction->GetMembers())
        {
            member->UpdateDistanceToCapital();
            member->UpdateColonialMap();
        }

        auto &neighbours = polity->GetSecederNeighbours();
        for(auto &neighbour : neighbours)
        {
            neighbour->UpdateDistanceToCapital();
            neighbour->UpdateColonialMap();
        }

        polity::PolityAllocator::Get()->FreeFaction(polity, faction);

        return newPolity;
    }

    const settlement::Settlement *WorldScene::GetFoundedSettlement() const 
    {
        return foundedSettlement;
    }

    void WorldScene::Render()
    {
        WorldTileModel::Get()->Render();
    }

    void WorldScene::HandleEnable() 
    {
        time.StopTime();
    }

    void WorldScene::HandleDisable() 
    {
        time.StopTime();
    }

    void WorldScene::HandleBattleRoundEnded()
    {
        static int roundCount = 0;
            
        roundCount++;
        if(roundCount == BATTLE_ROUNDS_PER_TICK)
        {
            roundCount = 0;

            Refresh();
        }
    }

    container::Grid <WorldTile> &WorldScene::GetTiles() const 
    {
        return worldMap->tiles;
    }
}