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
    InputHandler::OnRightMouseClick.Add(this, &CheckCharacterMovement);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &HandleSpacePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, this, &HandleOnePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, this, &HandleTwoPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, this, &HandleThreePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, this, &HandleFourPressed);

    selectedCharacter = nullptr;

    camera = battleScene->GetCamera();

    battleScene->OnEnabled.Add(this, &HandleSceneEnabled);
}

void BattleController::HandleSceneEnabled()
{
    SelectCharacter(battleScene->GetActingCharacter());
}

void BattleController::CheckCharacterMovement()
{
    if(hoveredTile == nullptr)
        return;

    if(selectedCharacter == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

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

void BattleController::SelectCharacter(Character* newlySelected)
{
    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Deselect();
    }

    selectedCharacter = newlySelected;

    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Select();
    }

    OnCharacterSelected.Invoke();
}

void BattleController::TargetCharacter(Character* target)
{
    if(selectedCharacter == nullptr)
        return;

    if(target == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

    if(selectedCharacter->CanAct(*target) == false)
        return;

    lastActionData = selectedCharacter->Act(*target);

    OnCharacterActed.Invoke();
}

BattleController * BattleController::Get()
{
    if(instance == nullptr)
    {
        instance = new BattleController();
    }

    return instance;
}