#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "InstructionInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface;

void InstructionInfoSet::HandleConfigure()
{
    infos.Initialize(64);

    for(int i = 0; i < infos.GetCapacity(); ++i)
    {
        auto info = ElementFactory::BuildElement <InstructionInfo>
        (
            {drawOrder_, {this}, {"WorkHammer", false}, Opacity(1.0f)},
            1
        );
        info->AdjustSizeToTexture();
        info->Disable();

        *infos.Add() = info;
    }

    WorldController::Get()->OnWorkerPlaceModeToggled += {this, &InstructionInfoSet::HandlePlaceModeEntered};

    polity::HumanMind::Get()->OnTileWorkerChanged += {this, &InstructionInfoSet::HandlePlaceModeEntered};
}

void InstructionInfoSet::HandlePlaceModeEntered()
{
    for(auto &info : infos)
    {
        info->Disable();
    }

    if(WorldController::Get()->IsWorkerPlaceModeActive() == true)
    {
        Enable();

        auto info = infos.GetStart();
        
        const auto instructions = polity::HumanMind::Get()->GetSettlementInstructions();
        if(instructions == nullptr)
            return;

        for(auto &instruction : *instructions)
        {
            if(instruction.PlaceType != polity::WorkInstruction::TILE)
                continue;

            (*info)->Setup(instruction.Place.Tile, instruction.Priority);
            (*info)->Enable();

            info++;
        }
    }
    else
    {
        Disable();
    }
}

void InstructionInfo::HandleConfigure()
{
    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {this}, {"WhiteDotBackdrop", false}}
    );
    counter->Enable();
}

void InstructionInfo::HandleUpdate()
{
    auto zoomFactor = RenderManager::GetCamera(Cameras::WORLD)->GetZoomFactor();

    SetTextureScale(Scale2(0.7f / zoomFactor));

    if(tile->IsWorked)
    {
        SetOpacity(1.0f);
        counter->SetOpacity(1.0f);
    }
    else
    {
        SetOpacity(0.5f);
        counter->SetOpacity(0.5f);
    }

    counter->Setup(Scale2(0.35f / zoomFactor), "Large");
}

void InstructionInfo::Setup(settlement::SettlementTile *tile, int priority)
{
    this->tile = tile;

    FollowWorldPosition(&tile->Tile->Position, Cameras::WORLD, Scale2(0.0f));

    counter->Setup(priority + 1);
}