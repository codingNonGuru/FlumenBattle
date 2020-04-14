#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"
#include <iostream>
BattleState * BattleState::instance = nullptr;

BattleState::BattleState()
{
    scene = BattleScene::Get();
    scene->Initialize();
}

void BattleState::HandleEnter() 
{
    scene->Enable();
}

void BattleState::HandleExit() 
{

}