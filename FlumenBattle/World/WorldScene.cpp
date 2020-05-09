#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"

#define MAXIMUM_GROUP_COUNT 64

#define MAXIMUM_BATTLE_COUNT 32

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
        battles.Initialize(MAXIMUM_BATTLE_COUNT);

        groups.Initialize(MAXIMUM_GROUP_COUNT);

        group::GroupFactory::Create(groups, {GroupTypes::PLAYER, RaceTypes::HUMAN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::ORC});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::GOBLIN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::ELF});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::HUMAN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::GNOME});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::HALFLING});

        time = WorldTime(230, 63, 14);
        timeSpeed = 1;
        isTimeFlowing = false;

        playerGroup = groups.GetStart();

        //battle = nullptr;
    }

    void WorldScene::SpeedUpTime()
    {
        timeSpeed--;
        utility::Clamp(timeSpeed, 1, 5);
    }

    void WorldScene::SlowDownTime()
    {
        timeSpeed++;
        utility::Clamp(timeSpeed, 1, 5);
    }

    void WorldScene::Update() 
    {
        OnUpdateStarted->Invoke();

        if(isTimeFlowing == false)
            return;

        AWAIT([this]() -> float 
        {
            switch(timeSpeed)
            {
                case 5: return 0.05f;
                case 4: return 0.1f;
                case 3: return 0.2f;
                case 2: return 0.5f;
                case 1: return 1.0f;
            }
        } ())

        time++;

        for(auto &group : groups)
        {
            group.DetermineAction();
        }

        for(auto &group : groups)
        {
            group.PerformAction();
        }
    }

    void WorldScene::StartBattle(group::Group *first, group::Group *second)
    {
        *battles.Add() = Battle(first, second);
        //battle = new Battle(first, second);
    }

    void WorldScene::Render()
    {

    }

    void WorldScene::HandleEnable() 
    {
        isTimeFlowing = false;
    }

    void WorldScene::HandleDisable() 
    {
        isTimeFlowing = false;
    }
}