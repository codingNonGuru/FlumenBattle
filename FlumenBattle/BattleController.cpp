#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Core/Task.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/ArtificialController.h"
#include "FlumenBattle/HumanController.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/BattleTile.h"

BattleController * BattleController::instance = nullptr;

void BattleController::Initialize()
{
    battleScene = BattleScene::Get();

    playerControlledGroup = battleScene->groups.Get(0);
    computerControlledGroup = battleScene->groups.Get(1);

    selectedCharacter = nullptr;

    HumanController::Get()->Initialize();

    battleScene->OnEnabled.Add(this, &HandleSceneEnabled);
}

void BattleController::HandleSceneEnabled()
{
    SelectCharacter(battleScene->GetActingCharacter());

    DetermineCharacterController();
}

void BattleController::DetermineCharacterController()
{
    auto humanController = HumanController::Get();
    if(selectedCharacter->GetGroup() == playerControlledGroup)
    {
        if(!isPlayerInputEnabled)
        {
            humanController->EnablePlayerInput();
        }

        isPlayerInputEnabled = true;
    }
    else
    {
        if(isPlayerInputEnabled)
        {
            humanController->DisablePlayerInput();
        }

        isPlayerInputEnabled = false;
        
        TaskManager::Add()->Initialize(&ArtificialController::Get(), &ArtificialController::UpdateCharacter, 0.7f);
    }
}

void BattleController::Move()
{
    if(targetedTile == nullptr)
        return;

    if(selectedCharacter == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCharacter) == false)
        return;

    selectedCharacter->Move(targetedTile);
}

void BattleController::SelectSubaction(Integer actionIndex)
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    auto currentIndex = selectedCharacter->GetSelectedSubactionIndex();
    if(selectedCharacter->SelectActionOption(actionIndex))
    {
        if(actionIndex != currentIndex)
        {
            targetedCharacter = nullptr;
        }

        OnSubactionSelected.Invoke();
    }
}

void BattleController::SelectAction(Integer actionIndex)
{
    auto currentIndex = selectedCharacter->GetSelectedActionIndex();
    if(selectedCharacter->SelectAction(actionIndex))
    {
        if(actionIndex != currentIndex)
        {
            targetedCharacter = nullptr;
        }

        OnActionSelected.Invoke();
    }
}

void BattleController::Act()
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
}

BattleController * BattleController::Get()
{
    if(instance == nullptr)
    {
        instance = new BattleController();
    }

    return instance;
}