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
    GroupFactory::Create(groups, {RaceTypes::HUMAN});
    GroupFactory::Create(groups, {RaceTypes::GOBLIN});
    GroupFactory::Create(groups, {RaceTypes::ELF});
    GroupFactory::Create(groups, {RaceTypes::HUMAN});
    GroupFactory::Create(groups, {RaceTypes::GNOME});
    GroupFactory::Create(groups, {RaceTypes::HALFLING});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_G, this, &WorldScene::HandleStateExit);
}

void WorldScene::HandleStateExit()
{
    battle = new Battle(groups.Get(0), groups.Get(1));

    BattleState::Get()->Enter();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_G, this, &WorldScene::HandleStateExit);
}

void WorldScene::Update() 
{
    /*if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_G) && !InputHandler::WasPressed(SDL_Scancode::SDL_SCANCODE_G))
    {
        battle = new Battle(groups.Get(0), groups.Get(1));

        BattleState::Get()->Enter();
    }*/
}

void WorldScene::Render()
{

}