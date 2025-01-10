#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "ImprovementInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldTile.h"

using namespace world::interface;

static const auto BASE_SIZE = Scale2(96.0f, 32.0f);

void ImprovementInfoSet::HandleConfigure()
{
    infos.Initialize(16);

    for(int i = 0; i < infos.GetCapacity(); ++i)
    {
        auto info = ElementFactory::BuildElement <ImprovementInfo>
        (
            {drawOrder_, {this}, {false}, Opacity(1.0f)},
            1
        );
        info->Disable();

        *infos.Add() = info;
    }

    WorldController::Get()->OnTileDevelopModeToggled += {this, &ImprovementInfoSet::HandleImproveModeEntered};

    polity::HumanMind::Get()->OnImprovementStarted += {this, &ImprovementInfoSet::HandleImproveModeEntered};
}

void ImprovementInfoSet::HandleImproveModeEntered()
{
    for(auto &info : infos)
    {
        info->Disable();
    }

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    if(WorldController::Get()->IsTileDevelopModeActive() == true && playerSettlement->IsImprovingAnyTile() == true)
    {
        Enable();

        auto info = infos.GetStart();

        (*info)->Setup(&playerSettlement->GetCurrentImprovement());
        (*info)->Enable();
    }
    else
    {
        Disable();
    }
}

void ImprovementInfo::HandleConfigure()
{
    progressBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {drawOrder_ + 1, {this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    progressBar->Enable();
}

void ImprovementInfo::HandleUpdate()
{
    if(target->Tile == nullptr)
    {
        Disable();
        return;
    }

    auto zoomFactor = RenderManager::GetCamera(Cameras::WORLD)->GetZoomFactor();
    if(zoomFactor > 1.2f)
    {
        progressBar->Disable();

        SetOpacity(0.0f);
    }
    else
    {
        progressBar->Enable();

        SetOpacity(1.0f);

        auto offset = Position2(0.0f, 20.0f) / zoomFactor;
        progressBar->SetBasePosition(offset);

        SetTextureScale(1.0f / zoomFactor);

        if(zoomFactor < 0.5f)
        {
            zoomFactor = 0.5f;
        }

        auto size = BASE_SIZE * (0.4f / zoomFactor);
        progressBar->SetSize(size);

        const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

        progressBar->SetProgress(playerSettlement->GetBuildingProduction()->GetProgressRatio());
    }
}

void ImprovementInfo::Setup(const settlement::ImprovementTarget *target)
{
    this->target = target;

    FollowWorldPosition(&target->Tile->Tile->Position, Cameras::WORLD, Scale2(0.0f));

    auto type = settlement::TileImprovementFactory::Get()->BuildImprovementType(target->ImprovementType);
    SetTexture(type->TextureName);
}