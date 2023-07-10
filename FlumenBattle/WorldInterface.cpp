#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/WorldHoverInfo.h"
#include "FlumenBattle/World/GroupEngageMenu.h"
#include "FlumenBattle/World/SettlementLabel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world;

WorldInterface::WorldInterface()
{
    canvas = ElementFactory::BuildCanvas();

    decisionMenu = ElementFactory::BuildElement<WorldDecisionMenu>(
        {Size(1080, 220), DrawOrder(3), {Position2(0.0f, 420.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement<WorldInfoPanel>(
        {Size(1900, 100), DrawOrder(3), {Position2(0.0f, -480.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    infoPanel->Enable();

    hoverInfo = ElementFactory::BuildElement<WorldHoverInfo>(
        {Size(390, 590), DrawOrder(3), {Position2(760.0f, 240.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    hoverInfo->Enable();

    engageMenu = ElementFactory::BuildElement<GroupEngageMenu>(
        {Size(900, 360), DrawOrder(3), {Position2(0.0f, 360.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    engageMenu->Disable();

    settlementLabels.Initialize(64);
    for(int i = 0; i < 64; i++)
    {
        auto settlementLabel = ElementFactory::BuildElement<settlement::SettlementLabel>(
            {Size(200, 170), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.4f)}
        );
        settlementLabel->Disable();
        *settlementLabels.Add() = settlementLabel;
    }
}

void WorldInterface::Initialize()
{
    auto &settlements = WorldScene::Get()->GetSettlements();

    auto settlementLabel = settlementLabels.GetStart();
    for(auto &settlement : settlements)
    {
        (*settlementLabel)->SetSettlement(&settlement);
        (*settlementLabel)->Enable();
        settlementLabel++;
    }

    *WorldScene::Get()->OnPlayerEncounterInitiated += {this, &WorldInterface::HandlePlayerEncounter};

    *WorldScene::Get()->OnPlayerEncounterFinished += {this, &WorldInterface::HandlePlayerDisengage};

    *WorldScene::Get()->OnPlayerBattleStarted += {this, &WorldInterface::HandlePlayerBattle};

    *WorldScene::Get()->OnPlayerBattleEnded += {this, &WorldInterface::HandleBattleEnded};
}

void WorldInterface::Enable()
{
    canvas->Enable();
}

void WorldInterface::Disable()
{
    canvas->Disable();
}

void WorldInterface::HandlePlayerEncounter()
{
    decisionMenu->Disable();

    engageMenu->Enable();
}

void WorldInterface::HandlePlayerDisengage()
{
    decisionMenu->Enable();

    engageMenu->Disable();
}

void WorldInterface::HandlePlayerBattle()
{
    engageMenu->Disable();
}

void WorldInterface::HandleBattleEnded()
{
    engageMenu->Enable();
}

void WorldInterface::Update()
{
    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    if(camera->GetZoomFactor() > 2.0f)
    {
        for(auto label = settlementLabels.GetStart(); label != settlementLabels.GetEnd(); label++)
        {
            (*label)->Disable();
        }
    }
    else
    {
        for(auto labelIterator = settlementLabels.GetStart(); labelIterator != settlementLabels.GetEnd(); labelIterator++)
        {
            auto label = *labelIterator;
            if(label->HasSettlement() == false)
                continue;

            label->Enable();
        }
    }
}