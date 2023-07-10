#include "FlumenCore/Time.hpp"
#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleInterface.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/CombatGroup.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/Encounter.h"

static Camera* camera = nullptr;

static const Float CAMERA_PAN_SPEED = 1500.0f;

static const Float CAMERA_ZOOM_SPEED = 10.0f;

static const Length BATTLE_MAP_SIZE = 55;

BattleScene::BattleScene() 
{
    battleMap = new BattleMap(BATTLE_MAP_SIZE);

    playerGroup = new CombatGroup();
    computerGroup = new CombatGroup();

    turnOrder.Initialize(256);
}

void BattleScene::Initialize()
{
    auto centerTile = battleMap->GetCenterTile();
    Integer3 offset = {-4, 12, -8};

    auto GetComputerGroup = [this]
    {
        //auto battle = world::WorldScene::Get()->GetPlayerBattle();
        auto battle = world::WorldController::Get()->GetPlayerBattle();
        if(world::WorldScene::Get()->GetPlayerGroup() == battle->GetFirst())
            return battle->GetSecond();
        else
            return battle->GetFirst();
    };  

    playerGroup->Initialize(world::WorldScene::Get()->GetPlayerGroup(), centerTile->GetNeighbor(offset));
    computerGroup->Initialize(GetComputerGroup(), centerTile->GetNeighbor(-offset));

    camera = RenderManager::GetCamera(Cameras::BATTLE);

    OnInitialized.Invoke();
}

void BattleScene::HandleEnable()
{
    DetermineTurnOrder();

    OnEnabled.Invoke();
}

void BattleScene::DetermineTurnOrder()
{
    turnOrder.Reset();
    for(auto group : {playerGroup, computerGroup})
    {    
        for(auto combatant = group->combatants.GetStart(); combatant != group->combatants.GetEnd(); ++combatant)
        {
            auto initiative = combatant->RollInitiative();
            *turnOrder.Allocate() = {combatant, initiative};
        }
    }

    turnOrder.SortDescendantly();

    turn = turnOrder.GetStart();

    turn->Combatant->StartTurn();
}

void BattleScene::EndTurn()
{
    while(true)
    {
        turn++;

        bool hasRoundEnded = turn == turnOrder.GetEnd();
        if(hasRoundEnded)
        {
            turn = turnOrder.GetStart();

            OnRoundEnded.Invoke();
        }

        if(turn->Combatant->IsAlive())
        {
            break;
        }

        if(turn->Combatant->IsSavingAgainstDeath())
        {
            turn->Combatant->SaveAgainstDeath();
        }
    }
}

void BattleScene::Update()
{
    auto panSpeed = CAMERA_PAN_SPEED * Time::GetDelta();

    if(InputHandler::IsPressed(SDL_SCANCODE_UP))
    {
        camera->Translate(Direction3(0.0f, -panSpeed, 0.0f));
    }
    else if(InputHandler::IsPressed(SDL_SCANCODE_DOWN))
    {
        camera->Translate(Direction3(0.0f, panSpeed, 0.0f));
    }

    if(InputHandler::IsPressed(SDL_SCANCODE_LEFT))
    {
        camera->Translate(Direction3(-panSpeed, 0.0f, 0.0f));
    }
    else if(InputHandler::IsPressed(SDL_SCANCODE_RIGHT))
    {
        camera->Translate(Direction3(panSpeed, 0.0f, 0.0f));
    }

    auto zoomSpeed = CAMERA_ZOOM_SPEED * Time::GetDelta();

    if(InputHandler::GetMouse().ScrollUp_)
    {
        camera->Zoom(1.0f - zoomSpeed);
    }
    else if(InputHandler::GetMouse().ScrollDown_)
    {
        camera->Zoom(1.0f + zoomSpeed);
    }
}

void BattleScene::Render()
{
    BattleTileModel::Get()->Render();
}

void BattleScene::HandleDisable()
{
    for(auto tile = battleMap->tiles.GetStart(); tile != battleMap->tiles.GetEnd(); ++tile)
    {
        tile->Combatant = nullptr;
    }

    //auto battle = world::WorldController::Get()->GetPlayerBattle();
    //battle->Finish();
}

bool BattleScene::IsCharactersTurn(Combatant *character) const
{
    return character == turn->Combatant;
}

Camera * BattleScene::GetCamera() const
{
    return camera;
}
