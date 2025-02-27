#include <mutex>

#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Thread/ThreadManager.h"
#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Disaster/Earthquake.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/SettlementFactory.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/World/Polity/PolityAllocator.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/SimulationMap.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Interface/Popup/PopupManager.h"
#include "FlumenBattle/World/WorldUpdateHandler.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Character/NameGenerator.h"
#include "FlumenBattle/World/Polity/Neighbor.h"

#define AWAIT(length) \
    static float timer = 0.0f;\
    timer += Time::GetDelta();\
    if(timer < length)\
        return;\
    else\
        timer = 0.0f;\

namespace world
{
    static auto politiesAffectedByLinkChange = container::Array <polity::Polity *> (256);

    WorldScene::WorldScene()
    {}

    void WorldScene::Initialize()
    {
        time.Initialize(230, 271, 14);

        battle::BattleScene::Get()->OnRoundEnded += {this, &WorldScene::HandleBattleRoundEnded};

        WorldInterface::Get()->Initialize();

        group::HumanMind::Get()->Enable();    

        character::NameGenerator::Get()->GenerateNamePool();

        Engine::OnLoopCycleEnded += [] 
        {
            WorldScene::Get()->ownershipChangeQueue.Reset();
        };
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
        if(!time)
            return;

        AWAIT(time.GetStep())

        OnUpdateStarted.Invoke();

        //ownershipChangeQueue.Reset();

        auto refreshCount = time.FlowSpeed == 5 ? 2 : 1;
        for(int i = 0; i < refreshCount; ++i)
        {
            Refresh();
        }
    }

    static auto factionDecisions = container::Array <polity::FactionDecision> (64);

    struct SettlementBuffer
    {
        container::Array <settlement::Settlement *> Settlements;

        SettlementBuffer() {}

        SettlementBuffer(int capacity) : Settlements(capacity) {}
    };

    struct ResultData
    {
        int Value {0};
    };

    void WorldScene::Refresh()
    {
        static auto index = 0;
        static auto durationSum = 0;

        auto startClock = high_resolution_clock::now();

        time++;

        WorldUpdateHandler::Get()->ResetAllData();

        politiesAffectedByLinkChange.Reset();

        auto refreshBattles = [this] 
        {
            static auto finishedBattles = Array <group::Encounter *> (battles->GetCapacity());

            for(auto &battle : *battles)
            {
                battle.Update();

                if(battle.IsOngoing() == false)
                {
                    *finishedBattles.Allocate() = &battle;
                }
            }

            for(auto &battle : finishedBattles)
            {
                battle->Terminate();

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

        auto refreshSettlementsThreaded = [this]
        {
            constexpr auto threadCount = engine::ThreadManager::GetThreadCount();

            static auto bufferCapacity = (settlements->GetCapacity() / threadCount) + 1;

            static SettlementBuffer buffers[threadCount];
            static auto bufferInitialize = [&] -> bool {
                for(auto &buffer : buffers)
                    buffer.Settlements.Initialize(bufferCapacity);

                return;
            } ();

            for(auto &buffer : buffers)
                buffer.Settlements.Reset();

            const auto sizePerBuffer = settlements->GetSize() / threadCount;
            auto sizeToBeCovered = sizePerBuffer;

            auto index = 0;
            auto bufferIndex = 0;
            for(auto &settlement : *settlements)
            {
                if(index == sizeToBeCovered)
                {
                    sizeToBeCovered += sizePerBuffer + 1;
                    bufferIndex++;
                }

                *buffers[bufferIndex].Settlements.Add() = &settlement;

                index++;
            }

            static engine::ThreadResultData <ResultData> results;
            for(auto i = 0; i < threadCount; ++i)
                results.GetResult(i).Value = 0;

            for(int i = 0; i < threadCount; ++i)
            {
                engine::ThreadManager::Get()->LaunchSyncThread([] (const SettlementBuffer &input, ResultData &output) 
                {
                    for(auto &settlement : input.Settlements)
                    {
                        output.Value++;
                        settlement->Update();

                    }
                }, buffers[i], results);
            }

            engine::ThreadManager::Get()->AwaitThreadFinish();

            //std::cout << settlements->GetSize() << " ---- ";
            //for(auto i = 0; i < threadCount; ++i)
                //std::cout << results.GetResult(i).Value << " ";
            //std::cout<<"\n";
        };

        auto refreshSettlements = [this]
        {
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
                settlement.FinishTradeUpdate();
            }

            for(auto &settlement : *settlements)
            {
                settlement.UpdatePolitics();
            }
        };

        //refreshSettlementsThreaded();

        refreshSettlements();

        WorldUpdateHandler::Get()->ProcessExplorationRewardData();

        polity::HumanMind::Get()->ProcessWorldUpdateData();

        for(auto &settlement : *settlements)
        {
            if(settlement.IsCompletelyGone() == true && settlement.IsValid() == true)
            {
                DestroySettlement(&settlement);
            }

            if(settlement.ShouldBeDeleted() == true)
            {
                settlement::SettlementAllocator::Get()->Free(&settlement);
            }
        }

        //CheckSettlementEmergence();

        if(time.IsNewDay == true)
        {
            auto earthquakeCount = 10;
            for(int i = 0; i < earthquakeCount; ++i)
            {
                //disaster::EarthquakeGenerator::Get()->Generate();
            }
        }

        auto refreshPolities = [this]
        {
            auto startClock = high_resolution_clock::now();

            for(auto &polity : *polities)
            {
                polity.Decide();
            }

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

            static auto processedPolities = container::Array <polity::Polity *> (politiesAffectedByLinkChange.GetCapacity());

            processedPolities.Reset();

            for(auto &polity : politiesAffectedByLinkChange)
            {
                if(polity->IsValid() == false)
                    continue;

                if(processedPolities.Find(polity) != nullptr)
                    continue;

                polity->RecalculateLinks();

                *processedPolities.Add() = polity;
            }

            for(auto &polity : *polities)
            {
                if(polity.IsValid() == false)
                    continue;

                std::cout<<"Neighbors of "<<polity.GetRuler()->GetName()<<": ";
                for(auto &neighbor : polity.GetNeighbors())
                {
                    std::cout<<neighbor.Polity->GetRuler()->GetName()<<" ";
                }
                std::cout<<"\n";
            }
            std::cout<<"\n";

            for(auto &polity : *polities)
            {
                polity.CleanUp();
            }

            for(auto &polity : *polities)
            {
                if(polity.ShouldBeDeleted() == false)
                    continue;

                polity::PolityAllocator::Get()->FreePolity(&polity);
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - startClock);
            //std::cout <<"polity refresh duration " << duration.count() << "\n";
        };

        refreshPolities();

        SimulationMap::Get()->Update();

        OnRefreshCycleFinished.Invoke();

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - startClock);

        durationSum += duration.count();
        index++;

        if(index == 60)
        {
            std::cout << "update average duration " << durationSum / 60 << ", per settlement: " << float(durationSum / 60) / float(settlements->GetSize()) << "\n";

            index = 0;
            durationSum = 0;
        }
    }

    static group::Encounter *mostRecentBattle = nullptr;

    const group::Encounter *WorldScene::GetMostRecentEncounter()
    {
        return mostRecentBattle;
    }

    void WorldScene::InitiateEncounter(group::GroupCore *attacker, group::GroupCore *defender)
    {
        if(playerGroup == attacker || playerGroup == defender)
        {
            if(playerGroup == attacker)
            {
                group::GroupFactory::TransformIntoDeepGroup(defender);
            }
            else
            {
                group::GroupFactory::TransformIntoDeepGroup(attacker);
            }
        }

        auto battle = battles->Add();
        battle->Initialize(attacker, defender);

        mostRecentBattle = battle;

        interface::popup::PopupManager::Get()->AddPopup(PopupTypes::BATTLE_START);

        if(playerGroup == attacker || playerGroup == defender)
        {
            OnPlayerEncounterInitiated.Invoke();

            this->StopTime(1);
        }
    }

    void WorldScene::InitiateDefenceBypass()
    {
        playerGroup->SelectAction(group::GroupActions::BYPASS_DEFENCES);

        this->StartTime(1);
    }

    void WorldScene::InitiateBribeGarrison()
    {
        playerGroup->SelectAction(group::GroupActions::BRIBE_GARRISON);

        this->StartTime(1);
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

        OnPlayerBattleStarted.Invoke();

        this->StartTime(1);
    }

    void WorldScene::FinishPlayerBattle()
    {
        playerGroup->SelectAction(group::GroupActions::ENGAGE, {true});

        playerGroup->GetEncounter()->EndFighting();

        OnPlayerBattleEnded.Invoke();
    }

    void WorldScene::FinishPlayerEncounter()
    {
        playerGroup->SelectAction(group::GroupActions::DISENGAGE);

        playerGroup->GetEncounter()->Finish(playerGroup);

        OnPlayerEncounterFinished.Invoke();

        this->StartTime(1);
    }

    void WorldScene::AccomplishPlayerConquest(settlement::Settlement *settlement)
    {
        conqueredSettlement = settlement;

        auto otherPolity = conqueredSettlement->GetPolity();

        if(playerGroup->DoesRulePolity() == true)
        {
            const auto playerPolity = playerGroup->GetDomain();

            if(otherPolity->GetSettlements().GetSize() == 1)
            {
                otherPolity->MarkForDeletion();

                playerPolity->ExtendRealm(conqueredSettlement);
            }
            else
            {
                DiminishPolity(otherPolity, conqueredSettlement);

                playerPolity->ExtendRealm(conqueredSettlement);
            }

            auto &tiles = conqueredSettlement->GetTiles();
            for(auto &tile : tiles)
            {
                UpdateOwnershipChangeQueue(tile.Tile);
            }

            OnPlayerDomainGrew.Invoke();
        }
        else
        {
            if(otherPolity->GetSettlements().GetSize() == 1)
            {
                playerGroup->SetDomain(otherPolity);

                otherPolity->SetController(true);

                *politiesAffectedByLinkChange.Add() = otherPolity;
            }
            else
            {
                DiminishPolity(otherPolity, conqueredSettlement);

                auto newPolity = FoundPolity(settlement, true);

                playerGroup->SetDomain(newPolity);

                for(auto &settlement : otherPolity->GetSettlements())
                {
                    auto &tiles = settlement->GetTiles();
                    for(auto &tile : tiles)
                    {
                        UpdateOwnershipChangeQueue(tile.Tile);
                    }
                }
            }

            OnPlayerBecameRuler.Invoke();
        }

        OnPlayerConquest.Invoke();
    }

    void WorldScene::CheckSettlementEmergence()
    {
        static auto lastEmergenceDate = GetTime().GetTickCount();

        static const auto DAYS_BETWEEN_EMERGENCE = 3;

        auto timeElapsed = GetTime().GetTickCount() - lastEmergenceDate;
        if(timeElapsed > WorldTime::GetTicksFromDays(DAYS_BETWEEN_EMERGENCE))
        {
            auto settlement = settlements->GetRandom();
            auto tile = settlement->GetLocation()->GetTileRing(5).Tiles.GetRandom();
            if((*tile)->HasRelief(WorldReliefs::SEA) == false && (*tile)->IsBorderingOwnedTile() == false)
            {
                FoundSettlement(*tile, RaceTypes::GNOME, nullptr);

                lastEmergenceDate = GetTime().GetTickCount();
            }

            /*auto tile = worldMap->GetRandomLandTile();
            if(tile->IsBorderingOwnedTile() == false)
            {
                FoundSettlement(tile, RaceTypes::GNOME, nullptr);

                lastEmergenceDate = GetTime().GetTickCount();
            }*/
        }
    }

    static settlement::Settlement *foundedSettlement = nullptr;

    settlement::Settlement * WorldScene::FoundSettlement(tile::WorldTile *location, RaceTypes race, settlement::Settlement *mother)
    {
        auto settlement = settlement::SettlementFactory::Create({location, race});

        auto polity = mother != nullptr ? mother->GetPolity() : nullptr;
        if(polity == nullptr)
        {
            polity = FoundPolity(settlement, false);
        }
        else
        {
            polity->ExtendRealm(settlement);
        }

        if(mother != nullptr)
        {
            ForgePath(settlement, mother);
        }

        *politiesAffectedByLinkChange.Add() = polity;

        auto tiles = location->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE);
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

            *politiesAffectedByLinkChange.Add() = other->GetPolity();                        
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
        OnSettlementFounded.Invoke();

        return foundedSettlement;
    }

    settlement::Settlement *WorldScene::ForgePath(settlement::Settlement *from, settlement::Settlement *to, int complexityLimit)
    {
        auto pathData = utility::Pathfinder <tile::WorldTile>::Get()->FindPathDjikstra(from->GetLocation(), to->GetLocation(), MAXIMUM_COLONIZATION_RANGE - 2);
        
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

    void WorldScene::DestroySettlement(settlement::Settlement *settlement)
    {
        settlement->MarkForDeletion();

        settlement->GetLocation()->RemoveSettlement();

        for(auto &tile : settlement->GetTiles())
        {
            bool isSettlementCenter = settlement->GetLocation() == tile.Tile;

            tile.Tile->AddRuin(settlement, isSettlementCenter);
        }

        *politiesAffectedByLinkChange.Add() = settlement->GetPolity();

        for(auto &link : settlement->GetLinks())
        {
            *politiesAffectedByLinkChange.Add() = link.Other->GetPolity();

            link.Other->RemoveLink(settlement);
        }
    }

    polity::Polity *WorldScene::FoundPolity(settlement::Settlement *ruler, bool isPlayerControlled)
    {
        auto polity = polity::PolityAllocator::Get()->AllocatePolity();
        polity->Initialize(ruler, isPlayerControlled);

        return polity;
    }

    void WorldScene::DiminishPolity(polity::Polity *polity, settlement::Settlement *settlement)
    {
        polity->UndergoDivision(settlement);

        polity->SetRuler(*polity->GetSettlements().GetRandom());

        *politiesAffectedByLinkChange.Add() = polity;

        auto &neighbours = polity->GetSecederNeighbours();
        for(auto &neighbour : neighbours)
        {
            neighbour->UpdateDistanceToCapital();
            neighbour->UpdateColonialMap();

            *politiesAffectedByLinkChange.Add() = neighbour->GetPolity();
        }
    }

    polity::Polity *WorldScene::SplitPolity(polity::Faction *faction)
    {
        auto polity = faction->GetPolity();

        polity->UndergoDivision(faction);

        auto newPolity = FoundPolity(faction->GetLeader(), false);
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

        *politiesAffectedByLinkChange.Add() = newPolity;

        auto &neighbours = polity->GetSecederNeighbours();
        for(auto &neighbour : neighbours)
        {
            neighbour->UpdateDistanceToCapital();
            neighbour->UpdateColonialMap();

            *politiesAffectedByLinkChange.Add() = neighbour->GetPolity();
        }

        polity::PolityAllocator::Get()->FreeFaction(polity, faction);

        for(auto &settlement : newPolity->GetSettlements())
        {
            auto &tiles = settlement->GetTiles();

            for(auto &tile : tiles)
            {
                UpdateOwnershipChangeQueue(tile.Tile);
            }
        }

        return newPolity;
    }

    const settlement::Settlement *WorldScene::GetFoundedSettlement() const 
    {
        return foundedSettlement;
    }

    void WorldScene::Render()
    {
        render::WorldTileModel::Get()->Render();
    }

    void WorldScene::HandleEnable() 
    {
        time.StopTime();

        polities;

        OnSceneEnabled.Invoke();
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

    container::Grid <tile::WorldTile> &WorldScene::GetTiles() const 
    {
        return worldMap->tiles;
    }

    group::GroupCore *WorldScene::GetGroup(int uniqueId)
    {
        for(auto &group : *groups)
        {
            if(group.GetUniqueId() == uniqueId)
                return &group;
        }

        return nullptr;
    }

    const group::GroupBuffer WorldScene::GetNearbyGroups(tile::WorldTile *tile, int maximumGroupDistance)
    {
        return group::GroupBatchMap::Get()->GetNearbyGroups(tile, maximumGroupDistance);
    }

    const group::GroupBuffer WorldScene::GetGroupsInTile(tile::WorldTile *tile)
    {
        return group::GroupBatchMap::Get()->GetGroupsInTile(tile);
    }

    settlement::Settlement *WorldScene::GetConqueredSettlement() const
    {
        return conqueredSettlement;
    }

    void WorldScene::UpdateOwnershipChangeQueue(tile::WorldTile *tile)
    {
        *ownershipChangeQueue.Add() = tile;
    }

    polity::Polity *WorldScene::GetPlayerPolity() const
    {
        playerGroup->GetDomain();
    }

    settlement::Settlement *WorldScene::GetPlayerSettlement() const
    {
        playerGroup->GetCurrentSettlement();
    }
}