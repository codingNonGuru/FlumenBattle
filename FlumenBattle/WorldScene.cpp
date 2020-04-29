#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"

#define MAXIMUM_GROUP_COUNT 64

void WorldScene::Initialize()
{
    groups.Initialize(MAXIMUM_GROUP_COUNT);

    GroupFactory::Create(groups, {RaceTypes::HUMAN});
    GroupFactory::Create(groups, {RaceTypes::ORC});
    GroupFactory::Create(groups, {RaceTypes::GOBLIN});
    GroupFactory::Create(groups, {RaceTypes::ELF});
    GroupFactory::Create(groups, {RaceTypes::HUMAN});
    GroupFactory::Create(groups, {RaceTypes::GNOME});
    GroupFactory::Create(groups, {RaceTypes::HALFLING});

    hourCount = 561372;

    playerGroup = groups.GetStart();
}

void WorldScene::StartBattle()
{
    static auto index = 1;

    auto computerGroup = groups.Get(index++);

    battle = new Battle(playerGroup, computerGroup);

    hourCount += 1;
}

void WorldScene::Rest()
{
    hourCount += 1;

    playerGroup->Rest();
}

void WorldScene::Render()
{

}