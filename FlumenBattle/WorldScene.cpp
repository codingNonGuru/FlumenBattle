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
}

void WorldScene::HandleEnable() 
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_G, this, &WorldScene::HandleEnterBattle);
}

void WorldScene::HandleEnterBattle()
{
    static auto index = 0;

    auto playerGroup = groups.Get(index++);
    auto computerGroup = groups.Get(index++);

    battle = new Battle(playerGroup, computerGroup);

    BattleState::Get()->Enter();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_G, this, &WorldScene::HandleEnterBattle);
}

void WorldScene::Render()
{

}