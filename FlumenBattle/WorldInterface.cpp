#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/WorldHoverInfo.h"
#include "FlumenBattle/World/Group/GroupEngageMenu.h"
#include "FlumenBattle/World/SettlementLabel.h"
#include "FlumenBattle/World/PathLabel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Interface/InventoryMenu.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world;

WorldController *controller = nullptr;

WorldInterface::WorldInterface()
{
    canvas = ElementFactory::BuildCanvas();
    canvas->SetInteractivity(true);

    decisionMenu = ElementFactory::BuildElement<WorldDecisionMenu>(
        {Size(1080, 220), DrawOrder(6), {Position2(0.0f, 420.0f), canvas}, {"Sprite"}, Opacity(0.75f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement<WorldInfoPanel>(
        {Size(1900, 80), DrawOrder(6), {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, canvas}, {"Sprite"}, Opacity(0.75f)}
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

    inventoryMenu = ElementFactory::BuildElement <interface::InventoryMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(6), 
            {Position2(0.0f, 0.0f), canvas}, 
            {"Sprite"}, 
            Opacity(0.9f)
        }
    );
    inventoryMenu->Disable();

    settlementLabels.Initialize(128);
    for(int i = 0; i < settlementLabels.GetCapacity(); i++)
    {
        auto settlementLabel = ElementFactory::BuildElement <settlement::SettlementLabel>(
            {Size(220, 40), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"panel-border-005", "SlicedSprite"}, Opacity(0.8f)}
        );
        settlementLabel->Disable();
        *settlementLabels.Add() = settlementLabel;
    }

    hoverExtension = ElementFactory::BuildElement <settlement::HoverExtension>(
        {Size(210, 320), DrawOrder(3), {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nullptr}, {"Sprite"}, Opacity(0.6f)}
    );
    hoverExtension->Disable();

    travelLabel = ElementFactory::BuildText(
        {Size(200, 50), DrawOrder(3), {Position2(), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, canvas}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Plan your travel"}
    );

    /*pathLabels.Initialize(1024);
    for(int i = 0; i < 1024; i++)
    {
        auto label = ElementFactory::BuildElement<PathLabel>(
            {Size(30, 30), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.2f)}
        );
        label->Disable();
        *pathLabels.Add() = label;
    }*/
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

    controller = WorldController::Get();

    *controller->onInventoryPressed += {this, &WorldInterface::HandleInventoryPressed};

    *controller->onCharacterSelected += {this, &WorldInterface::HandleCharacterSelected};
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

void WorldInterface::HandleInventoryPressed()
{
    if(isInInventoryMode == true)
    {
        isInInventoryMode = false;
        inventoryMenu->Disable();
    }
    else
    {
        isInInventoryMode = true;
        inventoryMenu->Enable();

        auto selectionData = WorldController::Get()->GetSelectionData();
        infoPanel->HandleInventoryOpen(selectionData.Index);

        auto character = WorldScene::Get()->GetPlayerGroup()->GetCharacter(selectionData.Index);
        inventoryMenu->SelectCharacter(character);
    }
}

void WorldInterface::HandleCharacterSelected()
{
    auto selectionData = WorldController::Get()->GetSelectionData();
    infoPanel->SelectCharacter(selectionData.Index, isInInventoryMode);

    if(isInInventoryMode == true)
    {
        auto selectionData = WorldController::Get()->GetSelectionData();
        auto character = WorldScene::Get()->GetPlayerGroup()->GetCharacter(selectionData.Index);
        inventoryMenu->SelectCharacter(character);
    }
}

void WorldInterface::Update()
{
    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    if(camera->GetZoomFactor() > 1.0f)
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

    if(controller->IsTravelPlanActive() == true)
    {
        travelLabel->Enable();
    }
    else
    {
        travelLabel->Disable();
    }

    /*for(auto &label : pathLabels)
    {
        label->Disable();
    }*/

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