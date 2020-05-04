#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"

#define MAXIMUM_GROUP_COUNT 64

#define AWAIT(length) \
    static float timer = 0.0f;\
    timer += Time::GetDelta();\
    if(timer < length)\
        return;\
    else\
        timer = 0.0f;\

namespace world
{
    void WorldScene::Initialize()
    {
        groups.Initialize(MAXIMUM_GROUP_COUNT);

        group::GroupFactory::Create(groups, {GroupTypes::PLAYER, RaceTypes::HUMAN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::ORC});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::GOBLIN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::ELF});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::HUMAN});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::GNOME});
        group::GroupFactory::Create(groups, {GroupTypes::COMPUTER, RaceTypes::HALFLING});

        hourCount = 561372;
        //timeFlowRate = 0.05f;

        playerGroup = groups.GetStart();
    }

    void WorldScene::SpeedUpTime()
    {
        timeSpeed++;
        utility::Clamp(timeSpeed, 1, 5);
    }

    void WorldScene::SlowDownTime()
    {
        timeSpeed--;
        utility::Clamp(timeSpeed, 1, 5);
    }

    void WorldScene::Update() 
    {
        if(isTimeFlowing == false)
            return;

        AWAIT([this]() -> float 
        {
            switch(timeSpeed)
            {
                case 1:
                    return 0.05f;
                case 2:
                    return 0.1f;
                case 3:
                    return 0.2f;
                case 4:
                    return 0.5f;
                case 5:
                    return 1.0f;
            }
        } ())

        hourCount++;

        for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
        {
            group->DetermineAction();
        }

        for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
        {
            group->PerformAction();
        }
    }

    void WorldScene::StartBattle()
    {
        static auto index = 1;

        auto computerGroup = groups.Get(index++);

        battle = new Battle(playerGroup, computerGroup);

        hourCount += 1;
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