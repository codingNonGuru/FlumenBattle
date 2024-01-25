#include "FlumenCore/Time.hpp"
#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Character/Character.h"

static Camera* camera = nullptr;

static const Float CAMERA_PAN_SPEED = 1500.0f;

static const Float CAMERA_ZOOM_SPEED = 2.0f;

static const Float CAMERA_GRAB_SENSITIVITY = 1.7f;

static const Scale2 CAMERA_ZOOM_LIMIT = {0.3f, 3.5f};

static const Integer3 COMBAT_GROUP_OFFSET = {-4, 12, -8};

static const Length BATTLE_MAP_SIZE = 55;

static const auto screenGrabInputKey = SDL_Scancode::SDL_SCANCODE_LALT;

namespace battle
{
    BattleScene::BattleScene() 
    {
        battleMap = new BattleMap(BATTLE_MAP_SIZE);

        playerGroup = new CombatGroup();
        computerGroup = new CombatGroup();

        turnOrder.Initialize(256);
    }

    void BattleScene::Initialize()
    {
        camera = RenderManager::GetCamera(Cameras::BATTLE);
        camera->LimitZoom(CAMERA_ZOOM_LIMIT);

        BattleController::Get()->OnCharacterActed += {this, &BattleScene::HandleCombatantActed};

        OnInitialized.Invoke();
    }

    void BattleScene::HandleEnable()
    {
        auto centerTile = battleMap->GetCenterTile();
        
        auto GetComputerGroup = [this]
        {
            auto battle = world::WorldController::Get()->GetPlayerBattle();
            if(world::WorldScene::Get()->GetPlayerGroup() == battle->GetFirst())
                return battle->GetSecond();
            else
                return battle->GetFirst();
        };  

        playerGroup->Initialize(world::WorldScene::Get()->GetPlayerGroup(), centerTile->GetNeighbor(COMBAT_GROUP_OFFSET));
        computerGroup->Initialize(GetComputerGroup(), centerTile->GetNeighbor(-COMBAT_GROUP_OFFSET));

        DetermineTurnOrder();

        InputHandler::RegisterContinualEvent(screenGrabInputKey, {this, &BattleScene::HandleGrabPressed}, {this, &BattleScene::HandleGrabReleased});

        OnEnabled.Invoke();
    }

    void BattleScene::HandleGrabPressed()
    {
        isGrabbingScreen = true;
        grabStartPosition = InputHandler::GetMousePosition();

        grabPositionDelta = Position2();
    }

    void BattleScene::HandleGrabReleased()
    {
        isGrabbingScreen = false;
    }

    void BattleScene::HandleCombatantActed()
    {
        for(auto group : {playerGroup, computerGroup})
        {    
            for(auto combatant = group->combatants.GetStart(); combatant != group->combatants.GetEnd(); ++combatant)
            {
                auto character = combatant->GetCharacter();

                character->RefreshModifiers();
            }
        }
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
        OnUpdate.Invoke();

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
            camera->ZoomDynamically(zoomSpeed);
        }
        else if(InputHandler::GetMouse().ScrollDown_)
        {
            camera->ZoomDynamically(-zoomSpeed);
        }

        if(isGrabbingScreen == true)
        {
            camera->Translate({grabPositionDelta.x, grabPositionDelta.y, 0.0f});

            auto mouseDelta = InputHandler::GetMousePosition() - grabStartPosition;
            
            mouseDelta *= CAMERA_GRAB_SENSITIVITY;

            mouseDelta *= camera->GetZoomFactor();

            camera->Translate({-mouseDelta.x, -mouseDelta.y, 0.0f});

            grabPositionDelta = mouseDelta;
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

        InputHandler::UnregisterContinualEvent(screenGrabInputKey);
    }

    bool BattleScene::IsCharactersTurn(Combatant *character) const
    {
        return character == turn->Combatant;
    }

    Camera * BattleScene::GetCamera() const
    {
        return camera;
    }
}