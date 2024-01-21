#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Core/Task.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/ArtificialController.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/World/WorldState.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace battle;

BattleController * BattleController::instance = nullptr;

void BattleController::Initialize()
{
    battleScene = BattleScene::Get();

    selectedCombatant = nullptr;

    HumanController::Get()->Initialize();

    battleScene->OnEnabled += {this, &BattleController::HandleSceneEnabled};
}

void BattleController::HandleSceneEnabled()
{
    targetedCombatant = nullptr;

    targetedTile = nullptr;

    SelectCombatant(battleScene->GetActingCharacter());

    isPlayerInputEnabled = false;

    DetermineCharacterController();

    battleScene->OnEnabled -= {this, &BattleController::HandleSceneEnabled};
}

void BattleController::DetermineCharacterController()
{
    auto humanController = HumanController::Get();
    if(selectedCombatant->GetGroup() == battleScene->GetPlayerGroup())
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
        
        TaskManager::Add({0.7f, {&ArtificialController::Get(), &ArtificialController::UpdateCharacter}});
    }
}

void BattleController::Move()
{
    if(targetedTile == nullptr)
        return;

    if(selectedCombatant == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCombatant) == false)
        return;

    selectedCombatant->Move(targetedTile);
}

void BattleController::SelectSubaction(Integer actionIndex)
{
    auto character = selectedCombatant->character;

    auto currentIndex = character->GetSelectedSubactionIndex();
    if(character->SelectActionOption(actionIndex))
    {
        if(actionIndex != currentIndex)
        {
            targetedCombatant = nullptr;
        }

        OnSubactionSelected.Invoke();
    }
}

void BattleController::SelectAction(Integer actionIndex)
{
    auto character = selectedCombatant->character;

    auto currentIndex = character->GetSelectedActionIndex();
    if(character->SelectAction(actionIndex))
    {
        if(actionIndex != currentIndex)
        {
            targetedCombatant = nullptr;
        }

        OnActionSelected.Invoke();
    }
}

void BattleController::Act()
{
    if(selectedCombatant == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCombatant) == false)
        return;

    if(selectedCombatant->CanAct(targetedCombatant) == false)
        return;

    lastActionData = selectedCombatant->Act(targetedCombatant);

    OnCharacterActed.Invoke();

    targetedCombatant = nullptr;
}

void BattleController::EndTurn()
{
    targetedCombatant = nullptr;

    for(auto group : {battleScene->GetPlayerGroup(), battleScene->GetComputerGroup()})
    {
        bool isDead = true;

        auto &combatants = group->GetCombatants();
        for(auto combatant = combatants.GetStart(); combatant != combatants.GetEnd(); ++combatant)
        {
            if(combatant->IsAlive())
            {
                isDead = false;
                break;
            }
        }

        if(isDead)
        {
            if(isPlayerInputEnabled)
            {
                auto humanController = HumanController::Get();
                humanController->DisablePlayerInput();

                isPlayerInputEnabled = false;
            }

            OnBattleEnded.Invoke();
            return;
        }
    }

    battleScene->EndTurn();

    SelectCombatant(battleScene->GetActingCharacter());

    selectedCombatant->StartTurn();

    DetermineCharacterController();
}

void BattleController::SelectCombatant(Combatant *combatant)
{
    if(selectedCombatant != nullptr)
    {
        selectedCombatant->Deselect();
    }

    selectedCombatant = combatant;

    if(selectedCombatant != nullptr)
    {
        selectedCombatant->Select();
    }

    OnCharacterSelected.Invoke();
}

void BattleController::TargetCombatant(Combatant *target)
{
    if(selectedCombatant == nullptr)
        return;

    if(target == nullptr)
        return;

    if(battleScene->IsCharactersTurn(selectedCombatant) == false)
        return;

    if(selectedCombatant->CanAct(target))
    {
        targetedCombatant = target;
    }
    else
    {
        targetedCombatant = nullptr;    
    }
}

world::character::Character * BattleController::GetSelectedCharacter() const 
{
    return selectedCombatant->character;
}

world::character::Character * BattleController::GetTargetedCharacter() const 
{
    return targetedCombatant->character;
}

void BattleController::ExitBattle()
{
    if(battleScene->GetPlayerGroup()->IsAlive())
    {
        world::WorldState::Get()->Enter();

        world::WorldScene::Get()->FinishPlayerBattle();
    }
    else
    {
        Engine::ShutDown();
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