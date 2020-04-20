#include "FlumenBattle/WorldState.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/Group.h"

WorldState::WorldState() {}

void WorldState::Initialize()
{
    scene = WorldScene::Get();
    scene->Initialize();
}

void WorldState::HandleEnter()
{
    Initialize();

    scene->Enable();
}

void WorldState::HandleExit() 
{
    scene->Disable();
}
