#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Core/Task.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/ArtificialController.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"

BattleController * BattleController::instance = nullptr;

static Camera* camera = nullptr;

void BattleController::Initialize()
{
    battleScene = BattleScene::Get();

    playerControlledGroup = battleScene->groups.Get(0);
    computerControlledGroup = battleScene->groups.Get(1);

    InputHandler::OnInputUpdate.Add(this, &CheckTileSelection);
    InputHandler::OnLeftMouseClick.Add(this, &CheckCharacterMovement);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &HandleSpacePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, this, &HandleOnePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, this, &HandleTwoPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, this, &HandleThreePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, this, &HandleFourPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, this, &HandleMPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_T, this, &HandleTPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_A, this, &HandleAPressed);

    selectedCharacter = nullptr;

    isInitiatingMove = false;

    isInitiatingTargeting = false;

    camera = battleScene->GetCamera();

    battleScene->OnEnabled.Add(this, &HandleSceneEnabled);
}

void BattleController::HandleSceneEnabled()
{
    SelectCharacter(battleScene->GetActingCharacter());

    DetermineCharacterController();
}

void BattleController::DetermineCharacterController()
{
    if(selectedCharacter->GetGroup() == playerControlledGroup)
    {
        isPlayerInputEnabled = true;
    }
    else
    {
        isPlayerInputEnabled = false;
        //ArtificialController::Get().UpdateCharacter();
        TaskManager::Add()->Initialize(&ArtificialController::Get(), &ArtificialController::UpdateCharacter, 1.0f);
    }
}

void BattleController::CheckCharacterMovement()
{
    if(isInitiatingMove == false)
        return;

    if(hoveredTile == nullptr)
        return;

    if(selectedCharacter == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

    isInitiatingMove = false;

    selectedCharacter->Move(hoveredTile);
}

void BattleController::CheckTileSelection()
{
    auto mesh = MeshManager::GetMesh("Hex"); 

    auto battleMap = battleScene->GetBattleMap();
    for(auto tile = battleMap->tiles.GetStart(); tile != battleMap->tiles.GetEnd(); ++tile) 
    {
        bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(tile->Position, 0.0f), 33.0f); 
        if(isMouseOverTile)
        {
            hoveredTile = tile;
            return;
        }
    }

    hoveredTile = nullptr;
}

void BattleController::HandleSpacePressed()
{
    EndTurn();
}

void BattleController::HandleOnePressed()
{
    if(selectedCharacter == nullptr)
        return;

    ChangeActionSelection(0);
}

void BattleController::HandleTwoPressed()
{
    if(selectedCharacter == nullptr)
        return;

    ChangeActionSelection(1);
}

void BattleController::HandleThreePressed()
{
    if(selectedCharacter == nullptr)
        return;

    ChangeActionSelection(2);
}

void BattleController::HandleFourPressed()
{
    if(selectedCharacter == nullptr)
        return;

    ChangeActionSelection(3);
}

void BattleController::ChangeActionSelection(Integer actionIndex)
{
    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        auto currentSubactionIndex = selectedCharacter->GetSelectedSubactionIndex();
        if(selectedCharacter->SelectActionOption(actionIndex))
        {
            if(actionIndex != currentSubactionIndex)
            {
                targetedCharacter = nullptr;
                isInitiatingTargeting = false;
            }

            OnSubactionSelected.Invoke();
        }
    }
    else
    {
        auto currentActionIndex = selectedCharacter->GetSelectedActionIndex();
        if(selectedCharacter->SelectAction(actionIndex))
        {
            if(actionIndex != currentActionIndex)
            {
                targetedCharacter = nullptr;
                isInitiatingTargeting = false;
            }

            OnActionSelected.Invoke();
        }
    }
}

void BattleController::HandleMPressed()
{
    if(isInitiatingMove)
    {
        isInitiatingMove = false;
    }
    else
    {
        isInitiatingMove = true;
        isInitiatingTargeting = false;
    }
}

void BattleController::HandleTPressed()
{
    if(selectedCharacter->CanTarget() == false)
        return;

    if(isInitiatingTargeting)
    {
        isInitiatingTargeting = false;
    }
    else
    {
        isInitiatingTargeting = true;
        isInitiatingMove = false;
    }
}

void BattleController::HandleAPressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

    if(selectedCharacter->CanAct(targetedCharacter) == false)
        return;

    lastActionData = selectedCharacter->Act(targetedCharacter);

    OnCharacterActed.Invoke();

    targetedCharacter = nullptr;
}

void BattleController::EndTurn()
{
    targetedCharacter = nullptr;

    battleScene->EndTurn();

    SelectCharacter(battleScene->GetActingCharacter());

    selectedCharacter->StartTurn();

    DetermineCharacterController();
}

void BattleController::SelectCharacter(Character *character)
{
    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Deselect();
    }

    selectedCharacter = character;

    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Select();
    }

    OnCharacterSelected.Invoke();
}

void BattleController::TargetCharacter(Character *target)
{
    if(isInitiatingTargeting == false)
        return;

    if(selectedCharacter == nullptr)
        return;

    if(target == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

    if(selectedCharacter->CanAct(target))
    {
        targetedCharacter = target;
    }
    else
    {
        targetedCharacter = nullptr;    
    }

    isInitiatingTargeting = false;
}

BattleController * BattleController::Get()
{
    if(instance == nullptr)
    {
        instance = new BattleController();
    }

    return instance;
}