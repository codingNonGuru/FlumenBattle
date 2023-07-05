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
#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementFactory.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"

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
    }

    void WorldScene::Initialize()
    {
        playerGroup = group::GroupFactory::Create({group::GroupTypes::PLAYER, RaceTypes::HUMAN});
        playerGroup->SetTile(worldMap->GetCenterTile());

        time = WorldTime(230, 63, 14);

        BattleScene::Get()->OnRoundEnded += {this, &WorldScene::HandleBattleRoundEnded};

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

        Refresh();
    }

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
                battles->Remove(battle);
            }

            finishedBattles.Reset();
        };

        refreshBattles();

        auto refreshGroups = [this] 
        {
            GroupAllocator::Get()->PerformCleanup();

            for(auto &group : *groups)
            {
                group.DetermineAction();
            }

            for(auto &group : *groups)
            {
                group.PerformAction();
            }
        };

        refreshGroups();

        auto refreshSettlements = [this]
        {
            //if(time.MinuteCount != 0)
                //return;

            for(auto &settlement : *settlements)
            {
                settlement.Update();
            }
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
            for(auto &polity : polities)
            {
                polity.Update();
            }
        };

        refreshPolities();
    }

    void WorldScene::InitiateEncounter(group::Group *first, group::Group *second)
    {
        auto battle = battles->Add();
        battle->Initialize(first, second);

        if(playerGroup == first || playerGroup == second)
        {
            OnPlayerEncounterInitiated->Invoke();

            this->StopTime();
        }
    }

    void WorldScene::InitiatePlayerBattle()
    {
        playerGroup->SelectAction(group::GroupActions::FIGHT);

        OnPlayerBattleStarted->Invoke();
    }

    void WorldScene::FinishPlayerEncounter()
    {
        playerGroup->SelectAction(group::GroupActions::DISENGAGE);

        playerGroup->GetEncounter()->Finish();

        OnPlayerEncounterFinished->Invoke();
    }

    settlement::Settlement * WorldScene::FoundSettlement(WorldTile *location, Polity *polity)
    {
        auto settlement = settlement::SettlementFactory::Create({"Safehaven", location});
        if(polity == nullptr)
        {
            polity = polities.Add();
            polity->Initialize(settlement);
        }
        else
        {
            polity->ExtendRealm(settlement);
        }

        settlement->SetPolity(polity);
    }

    void WorldScene::Render()
    {
        WorldTileModel::Get()->Render();
    }

    void WorldScene::HandleEnable() 
    {
        time = false;
    }

    void WorldScene::HandleDisable() 
    {
        time = false;
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
}