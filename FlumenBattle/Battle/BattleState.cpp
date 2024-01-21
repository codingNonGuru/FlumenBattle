#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Battle/BattleTileModel.h"

using namespace battle;

static BattleTileModel *tileModel = nullptr;

BattleState::BattleState()
{
    scene = BattleScene::Get();

    controller = BattleController::Get();

    tileModel = BattleTileModel::Get();

    interface = BattleInterface::Get();
}

void BattleState::HandleEnter() 
{
    scene->Initialize();
    
    controller->Initialize();

    tileModel->Initialize();

    interface->Initialize();

    scene->Enable();
}

void BattleState::HandleExit() 
{
    interface->Disable();

    scene->Disable();
}