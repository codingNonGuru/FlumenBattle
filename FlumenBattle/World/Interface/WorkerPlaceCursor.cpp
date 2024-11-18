#include "SDL2/SDL.h"

#include "FlumenEngine/Interface/ElementFactory.h"

#include "WorkerPlaceCursor.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface;

int freeWorkerCount = 0;

void WorkerPlaceCursor::HandleConfigure()
{
    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(12.0f, 22.0f), this}, {"WhiteDotBackdrop", false}}
    );

    counter->Setup(&freeWorkerCount, Scale2(1.0f), "Medium");
    counter->Enable();
}

void WorkerPlaceCursor::HandleEnable() 
{
    SetTextureScale(Scale2(2.0f));

    SDL_ShowCursor(SDL_DISABLE);
}

void WorkerPlaceCursor::HandleDisable() 
{
    SDL_ShowCursor(SDL_ENABLE);
}

void WorkerPlaceCursor::HandleUpdate()
{
    if(WorldController::Get()->IsWorkerPlaceModeActive() == false)
    {
        Disable();
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = playerGroup->GetCurrentSettlement();
    if(playerSettlement == nullptr)
    {
        Disable();
        return;
    }

    freeWorkerCount = playerSettlement->GetFreeWorkerCount();
    if(freeWorkerCount == 0)
    {
        SetOpacity(0.5f);
    }
    else
    {
        SetOpacity(1.0f);
    }
}