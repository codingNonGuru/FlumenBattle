#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleController.h"

BattleState * BattleState::instance = nullptr;

BattleState::BattleState()
{
    scene = BattleScene::Get();
    scene->Initialize();
}

void BattleState::HandleEnter() 
{
    controller = BattleController::Get();
    controller->Initialize();

    scene->Enable();
}

void BattleState::HandleExit() 
{

}