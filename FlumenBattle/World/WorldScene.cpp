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
#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementFactory.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"

#define MAXIMUM_BATTLE_COUNT 32

#define WORLD_MAP_SIZE 51

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
    }

    void WorldScene::Initialize()
    {
        worldMap = new WorldMap(WORLD_MAP_SIZE);

        WorldGenerator::Get()->GenerateWorld(*this);

        polities.Initialize(64);

        settlements = &SettlementAllocator::Get()->settlements;

        auto findSettleLocation = [this]
        {
            while(true)
            {
                auto tile = worldMap->GetEmptyRandomTile();

                bool isSettlementNearby = false;
                for(auto &settlement : *settlements)
                {
                    auto distance = tile->GetDistanceTo(*settlement.GetLocation());
                    if(distance < 12)
                    {
                        isSettlementNearby = true;
                        break;
                    }
                }
                if(isSettlementNearby)
                    continue;

                if(tile->HasBiome(WorldBiomes::STEPPE) && tile->GetSettlement() == nullptr)
                    return tile;
            }
        };
        
        for(auto i = 0; i < 5; ++i)
        {
            auto location = findSettleLocation();
            FoundSettlement(location, nullptr);
        }

        groups = &GroupAllocator::Get()->groups;

        battles.Initialize(MAXIMUM_BATTLE_COUNT);

        playerGroup = group::GroupFactory::Create({group::GroupTypes::PLAYER, RaceTypes::HUMAN});
        playerGroup->SetTile(worldMap->GetCenterTile());

        group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::ORC})->SetTile(worldMap->GetEmptyRandomTile());
        //group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::GOBLIN})->SetTile(worldMap->GetEmptyRandomTile());
        //group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::ELF})->SetTile(worldMap->GetEmptyRandomTile());
        //group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::HUMAN})->SetTile(worldMap->GetEmptyRandomTile());
        //group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::GNOME})->SetTile(worldMap->GetEmptyRandomTile());
        //group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::HALFLING})->SetTile(worldMap->GetEmptyRandomTile());

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
            static auto finishedBattles = Array <Battle *> (MAXIMUM_BATTLE_COUNT);

            for(auto &battle : battles)
            {
                battle.Update();

                if(!battle.IsOngoing())
                {
                    *finishedBattles.Allocate() = &battle;
                }
            }

            for(auto &battle : finishedBattles)
            {
                battles.Remove(battle);
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

        auto refreshPolities = [this]
        {
            for(auto &polity : polities)
            {
                polity.Update();
            }
        };

        refreshPolities();
    }

    void WorldScene::StartBattle(group::Group *first, group::Group *second)
    {
        *battles.Add() = Battle(first, second);
    }

    Settlement * WorldScene::FoundSettlement(WorldTile *location, Polity *polity)
    {
        auto settlement = SettlementFactory::Create({"Safehaven", location});
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
        if(roundCount == BATTLE_ROUNDS_PER_MINUTE)
        {
            roundCount = 0;

            Refresh();
        }
    }
}