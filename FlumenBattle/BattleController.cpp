#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"

BattleController * BattleController::instance = nullptr;

static Camera* camera = nullptr;

void BattleController::Initialize()
{
    battleScene = BattleScene::Get();

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
    targetedCharacter = nullptr;

    battleScene->EndTurn();

    SelectCharacter(battleScene->GetActingCharacter());

    selectedCharacter->StartTurn();
}

void BattleController::HandleOnePressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        if(selectedCharacter->SelectActionOption(0))
        {
            OnSubactionSelected.Invoke();
        }
    }
    else
    {
        if(selectedCharacter->SelectAction(0))
        {
            OnActionSelected.Invoke();
        }
    }
}

void BattleController::HandleTwoPressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        if(selectedCharacter->SelectActionOption(1))
        {
            OnSubactionSelected.Invoke();
        }
    }
    else
    {
        if(selectedCharacter->SelectAction(1))
        {
            OnActionSelected.Invoke();
        }
    }
}

void BattleController::HandleThreePressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(selectedCharacter->SelectAction(2))
    {
        OnActionSelected.Invoke();
    }
}

void BattleController::HandleFourPressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(selectedCharacter->SelectAction(3))
    {
        OnActionSelected.Invoke();
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

    if(selectedCharacter->CanAct() == false)
        return;

    lastActionData = selectedCharacter->Act();

    OnCharacterActed.Invoke();

    targetedCharacter = nullptr;
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

    isInitiatingTargeting = false;

    targetedCharacter = target;
}

BattleController * BattleController::Get()
{
    if(instance == nullptr)
    {
        instance = new BattleController();
    }

    return instance;
}