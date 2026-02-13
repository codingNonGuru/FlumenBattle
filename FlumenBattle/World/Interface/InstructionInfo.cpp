#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "InstructionInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/Rule/ResourceHoverInfo.h"
#include "FlumenBattle/Config.h"

using namespace world::interface;

void InstructionInfoSet::HandleConfigure()
{
    static const auto MAX_SETTLEMENT_POPULATION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAX_SETTLEMENT_POPULATION).Integer;

    infos.Initialize(MAX_SETTLEMENT_POPULATION);

    for(int i = 0; i < infos.GetCapacity(); ++i)
    {
        auto info = ElementFactory::BuildElement <InstructionInfo>
        (
            {drawOrder_, {this}, {"WorkHammer", false}, Opacity(1.0f)},
            1
        );
        info->Disable();

        *infos.Add() = info;
    }

    resourceHoverInfo = ElementFactory::BuildElement <rule::ResourceHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );
    resourceHoverInfo->Disable();

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

            (*info)->Setup(&instruction);
            (*info)->Enable();

            info++;
        }
    }
    else
    {
        Disable();
    }
}

void InstructionInfoSet::HandleEnable()
{
    resourceHoverInfo->Enable();
    resourceHoverInfo->Setup();
}

void InstructionInfoSet::HandleDisable()
{
    resourceHoverInfo->Disable();
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

    SetTextureScale(Scale2(0.5f / zoomFactor));

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
    counter->SetBasePosition(Position2{7.0f, 10.0f} / zoomFactor);
}

void InstructionInfo::Setup(const polity::WorkInstruction *instruction)
{
    this->tile = instruction->Place.Tile;

    FollowWorldPosition(&tile->Tile->Position, Cameras::WORLD, Scale2(0.0f));

    counter->Setup(instruction->Priority + 1);

    SetTexture(instruction->Cohort->Race->TextureName);
}