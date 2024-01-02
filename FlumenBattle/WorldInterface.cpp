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
#include "FlumenBattle/World/PathLabel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"

using namespace world;

WorldInterface::WorldInterface()
{
    canvas = ElementFactory::BuildCanvas();

    decisionMenu = ElementFactory::BuildElement<WorldDecisionMenu>(
        {Size(1080, 220), DrawOrder(6), {Position2(0.0f, 420.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement<WorldInfoPanel>(
        {Size(1900, 100), DrawOrder(6), {Position2(0.0f, -480.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    infoPanel->Enable();

    hoverInfo = ElementFactory::BuildElement<WorldHoverInfo>(
        {Size(390, 590), DrawOrder(6), {Position2(760.0f, 240.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    hoverInfo->Enable();

    engageMenu = ElementFactory::BuildElement<GroupEngageMenu>(
        {Size(900, 360), DrawOrder(6), {Position2(0.0f, 360.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    engageMenu->Disable();

    settlementLabels.Initialize(256);
    for(int i = 0; i < settlementLabels.GetCapacity(); i++)
    {
        auto settlementLabel = ElementFactory::BuildElement<settlement::SettlementLabel>(
            {Size(200, 40), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.4f)}
        );
        settlementLabel->Disable();
        *settlementLabels.Add() = settlementLabel;
    }

    pathLabels.Initialize(1024);
    for(int i = 0; i < 1024; i++)
    {
        auto label = ElementFactory::BuildElement<PathLabel>(
            {Size(30, 30), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.2f)}
        );
        label->Disable();
        *pathLabels.Add() = label;
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

    *WorldScene::Get()->OnSettlementFounded += {this, &WorldInterface::HandleSettlementFounded};
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

void WorldInterface::HandleSettlementFounded()
{
    auto settlement = WorldScene::Get()->GetFoundedSettlement();
    for(auto labelIterator = settlementLabels.GetStart(); labelIterator != settlementLabels.GetEnd(); labelIterator++)
    {
        auto label = *labelIterator;
        if(label->HasSettlement() == false)
        {
            label->SetSettlement(settlement);
            label->Enable();
            break;
        }
    }
}

void WorldInterface::Update()
{
    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    if(camera->GetZoomFactor() > 1.7f)
    {
        for(auto label = settlementLabels.GetStart(); label != settlementLabels.GetEnd(); label++)
        {
            if((*label)->IsSettlementHovered() == true)
            {
                (*label)->Enable();
            }
            else
            {
                (*label)->Disable();
            }
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

    for(auto &label : pathLabels)
    {
        label->Disable();
    }

    auto worldMap = WorldScene::Get()->GetWorldMap();
    auto polity = WorldScene::Get()->GetPolities().Get(0);
    auto &interestMap = polity->GetInterestMap().GetTiles();

    /*int index = 0;
    for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile)
    {
        if(tile->PathData.IsVisited == true)
        {
            auto label = *pathLabels.Get(index);
            label->SetTile(tile);
            label->Enable();
            index++;
        }
    }*/

    /*int index = 0;
    for(auto tile = interestMap.GetStart(); tile != interestMap.GetEnd(); ++tile)
    {
        if(tile->GetOwner() == nullptr)
            continue;
    
        auto label = *pathLabels.Get(index);
        label->SetTile(worldMap->GetTile(tile->Coordinates), tile->GetDistance());
        label->Enable();
        index++;
    }*/
}