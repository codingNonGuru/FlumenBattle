#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTileModel.h"
#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"

#define MAXIMUM_BATTLE_COUNT 32

#define WORLD_MAP_SIZE 20

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

        groups = &GroupAllocator::Get()->groups;

        battles.Initialize(MAXIMUM_BATTLE_COUNT);

        playerGroup = group::GroupFactory::Create({GroupTypes::PLAYER, RaceTypes::HUMAN});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::ORC});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::GOBLIN});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::ELF});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::HUMAN});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::GNOME});
        group::GroupFactory::Create({GroupTypes::COMPUTER, RaceTypes::HALFLING});

        time = WorldTime(230, 63, 14);

        BattleScene::Get()->OnRoundEnded += {this, &WorldScene::HandleBattleRoundEnded};
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
    }

    void WorldScene::StartBattle(group::Group *first, group::Group *second)
    {
        *battles.Add() = Battle(first, second);
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