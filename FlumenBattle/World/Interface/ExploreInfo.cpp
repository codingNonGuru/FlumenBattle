#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "ExploreInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"

using namespace world::interface;

static const auto BASE_SIZE = Scale2(96.0f, 32.0f);

void ExploreInfoSet::HandleConfigure()
{
    infos.Initialize(16);

    for(int i = 0; i < infos.GetCapacity(); ++i)
    {
        auto info = ElementFactory::BuildElement <ExploreInfo>
        (
            {drawOrder_, {this}},
            1
        );
        info->Disable();

        *infos.Add() = info;
    }

    WorldController::Get()->OnExploreModeToggled += {this, &ExploreInfoSet::HandleExploreModeEntered};

    polity::HumanMind::Get()->OnExplorationStarted += {this, &ExploreInfoSet::HandleExploreModeEntered};
}

void ExploreInfoSet::HandleExploreModeEntered()
{
    for(auto &info : infos)
    {
        info->Disable();
    }

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    if(WorldController::Get()->IsExploreModeActive() == true && playerSettlement->IsExploring() == true)
    {
        Enable();

        auto info = infos.GetStart();
        
        auto &exploration = playerSettlement->GetCurrentExploration();

        (*info)->Setup(&exploration);
        (*info)->Enable();
    }
    else
    {
        Disable();
    }
}

void ExploreInfo::HandleConfigure()
{
    progressBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {drawOrder_ + 1, {this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    progressBar->Enable();
}

void ExploreInfo::HandleUpdate()
{
    if(exploration->Tile == nullptr)
    {
        Disable();
        return;
    }

    auto zoomFactor = RenderManager::GetCamera(Cameras::WORLD)->GetZoomFactor();
    if(zoomFactor > 1.2f)
    {
        progressBar->Disable();
    }
    else
    {
        progressBar->Enable();

        auto offset = Position2(0.0f, 20.0f) / zoomFactor;
        progressBar->SetBasePosition(offset);

        if(zoomFactor < 0.5f)
        {
            zoomFactor = 0.5f;
        }

        auto size = BASE_SIZE * (0.4f / zoomFactor);
        progressBar->SetSize(size);

        progressBar->SetProgress(exploration->GetProgressFactor());
    }
}

void ExploreInfo::Setup(const settlement::Exploration *exploration)
{
    this->exploration = exploration;

    FollowWorldPosition(&exploration->Tile->Position, Cameras::WORLD, Scale2(0.0f));
}