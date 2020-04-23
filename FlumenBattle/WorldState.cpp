#include "FlumenBattle/WorldState.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/Group.h"

WorldState::WorldState() 
{
    Initialize();
}

void WorldState::Initialize()
{
    scene = WorldScene::Get();
    scene->Initialize();
}

void WorldState::HandleEnter()
{
    scene->Enable();
}

void WorldState::HandleExit() 
{
    scene->Disable();
}
