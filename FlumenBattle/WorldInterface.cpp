#include <chrono>

#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

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
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Interface/InventoryMenu.h"
#include "FlumenBattle/World/Interface/ReputationMenu.h"
#include "FlumenBattle/World/Interface/SettlementMenu.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Interface/VendorCursor.h"
#include "FlumenBattle/World/Interface/ItemHoverInfo.h"
#include "FlumenBattle/World/Interface/ExplorationMenu.h"
#include "FlumenBattle/World/Interface/GroupHoverInfo.h"
#include "FlumenBattle/World/Interface/QuestPopup.h"
#include "FlumenBattle/World/Interface/QuestMenu.h"
#include "FlumenBattle/World/Interface/ActionPopup.h"
#include "FlumenBattle/World/Interface/RollPopup.h"
#include "FlumenBattle/World/Interface/MoneyPopup.h"
#include "FlumenBattle/World/Interface/ItemPopup.h"
#include "FlumenBattle/World/Interface/ProductionDecisionMenu.h"
#include "FlumenBattle/World/Interface/ConquestPopup.h"

using namespace world;

WorldController *controller = nullptr;

static const auto CONSOLE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_GRAVE;

static const auto MENU_CYCLE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_TAB;

#define ACTION_POPUP_CAPACITY 4

#define ROLL_POPUP_CAPACITY 16

auto popupTimestamp = std::chrono::steady_clock::now();

#define TIME_BETWEEN_FADING_POPUPS 300

WorldInterface::WorldInterface() : popupQueue(ROLL_POPUP_CAPACITY * 4)
{
    canvas = ElementFactory::BuildCanvas();
    canvas->SetInteractivity(true);

    decisionMenu = ElementFactory::BuildElement <WorldDecisionMenu>(
        {Size(1080, 220), DrawOrder(6), {Position2(0.0f, 420.0f), canvas}, {false}, Opacity(0.75f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement <WorldInfoPanel>(
        {Size(1900, 80), DrawOrder(6), {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, canvas}, {false}, Opacity(0.75f)}
    );
    infoPanel->Enable();

    hoverInfo = ElementFactory::BuildElement <WorldHoverInfo>(
        {Size(540, 420), DrawOrder(6), {canvas}, {false}, Opacity(0.5f)}
    );

    engageMenu = ElementFactory::BuildElement <GroupEngageMenu>(
        {Size(900, 360), DrawOrder(6), {Position2(0.0f, -5.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, canvas}, {false}, Opacity(0.75f)}
    );

    inventoryMenu = ElementFactory::BuildElement <interface::InventoryMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(7), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    reputationMenu = ElementFactory::BuildElement <interface::ReputationMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(7), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    questMenu = ElementFactory::BuildElement <interface::QuestMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(7), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    reputationMenu = ElementFactory::BuildElement <interface::ReputationMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(7), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    reputationMenu->Disable();

    settlementMenu = ElementFactory::BuildElement <interface::SettlementMenu>
    (
        {
            Size(320, 400), 
            DrawOrder(6), 
            {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    explorationMenu = ElementFactory::BuildElement <interface::ExplorationMenu>
    (
        {
            Size(320, 400), 
            DrawOrder(6), 
            {Position2(-5.0f, -5.0f), ElementAnchors::LOWER_RIGHT, ElementPivots::LOWER_RIGHT, canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    explorationMenu->Enable();

    settlementLabels.Initialize(128);
    for(int i = 0; i < settlementLabels.GetCapacity(); i++)
    {
        auto settlementLabel = ElementFactory::BuildElement <settlement::SettlementLabel>
        (
            {Size(220, 40), DrawOrder(3), {canvas}, {"panel-border-005", true}, Opacity(0.7f)}
        );
        settlementLabel->Disable();
        *settlementLabels.Add() = settlementLabel;
    }

    hoverExtension = ElementFactory::BuildElement <settlement::HoverExtension>
    (
        {Size(210, 320), DrawOrder(3), {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nullptr}, {false}, Opacity(0.6f)}
    );

    travelLabel = ElementFactory::BuildText
    (
        {Size(200, 50), DrawOrder(3), {Position2(0.0f, 200.0f), canvas}},
        {{"Medium"}, Color::RED * 0.5f, "Plan your travel"}
    );

    /*pathLabels.Initialize(1024);
    for(int i = 0; i < 1024; i++)
    {
        auto label = ElementFactory::BuildElement<PathLabel>(
            {Size(30, 30), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {false}, Opacity(0.2f)}
        );
        label->Disable();
        *pathLabels.Add() = label;
    }*/

    vendorCursor = ElementFactory::BuildElement <interface::VendorCursor>
    (
        {
            inventoryMenu->GetDrawOrder() + 5, 
            {canvas}, 
            {"Coin", false}
        }
    );
    vendorCursor->FollowMouse();

    itemHoverInfo = ElementFactory::BuildElement <interface::ItemHoverInfo>
    (
        {
            inventoryMenu->GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, canvas},
            {false},
            Opacity(0.5f)
        }
    );

    groupHoverInfo = ElementFactory::BuildElement <interface::GroupHoverInfo>
    (
        {
            DrawOrder(5),
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, canvas},
            {false},
            Opacity(0.7f)
        }
    );

    questPopup = ElementFactory::BuildElement <interface::QuestPopup>
    (
        {
            Size(480, 200), 
            DrawOrder(6), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    actionPopups.Initialize(ACTION_POPUP_CAPACITY);
    for(int i = 0; i < ACTION_POPUP_CAPACITY; ++i)
    {
        *actionPopups.Add() = ElementFactory::BuildElement <interface::ActionPopup>
        (
            {DrawOrder(6), {canvas}}
        );
    }

    actionPopups.Reset();

    rollPopups.Initialize(ROLL_POPUP_CAPACITY);
    for(int i = 0; i < ROLL_POPUP_CAPACITY; ++i)
    {
        *rollPopups.Add() = ElementFactory::BuildElement <interface::RollPopup>
        (
            {DrawOrder(6), {canvas}}
        );
    }

    rollPopups.Reset();

    moneyPopups.Initialize(ACTION_POPUP_CAPACITY);
    for(int i = 0; i < ACTION_POPUP_CAPACITY; ++i)
    {
        *moneyPopups.Add() = ElementFactory::BuildElement <interface::MoneyPopup>
        (
            {DrawOrder(6), {canvas}}
        );
    }

    moneyPopups.Reset();

    itemPopups.Initialize(ROLL_POPUP_CAPACITY);
    for(int i = 0; i < ROLL_POPUP_CAPACITY; ++i)
    {
        *itemPopups.Add() = ElementFactory::BuildElement <interface::ItemPopup>
        (
            {DrawOrder(6), {canvas}}
        );
    }

    itemPopups.Reset();

    productionMenu = ElementFactory::BuildElement <interface::ProductionDecisionMenu>
    (
        {
            Size(320, 400), 
            DrawOrder(6), 
            {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    conquestPopup = ElementFactory::BuildElement <interface::ConquestPopup>
    (
        {
            Size(480, 200), 
            DrawOrder(6), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    group::HumanMind::Get()->OnSpottingHovered += {this, &WorldInterface::HandleSpottingHovered};

    group::HumanMind::Get()->OnQuestStarted += {this, &WorldInterface::HandleQuestStarted};

    group::HumanMind::Get()->OnQuestFinished += {this, &WorldInterface::HandleQuestFinished};

    group::HumanMind::Get()->OnActionInitiated += {this, &WorldInterface::HandleActionInitiated};

    group::HumanMind::Get()->OnSkillCheckRolled += {this, &WorldInterface::HandleDiceRolled};

    group::HumanMind::Get()->OnPlayerWealthChanged += {this, &WorldInterface::HandlePlayerWealthChanged};

    group::HumanMind::Get()->OnItemAdded += {this, &WorldInterface::HandlePlayerItemChanged};

    group::HumanMind::Get()->OnItemSold += {this, &WorldInterface::HandlePlayerItemChanged};

    group::HumanMind::Get()->OnSettlementEntered += {this, &WorldInterface::HandleSettlementEntered};

    group::HumanMind::Get()->OnSettlementExited += {this, &WorldInterface::HandleSettlementExited};

    WorldScene::Get()->OnPlayerConquest += {this, &WorldInterface::HandlePlayerConquest};
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

    WorldScene::Get()->OnPlayerEncounterInitiated += {this, &WorldInterface::HandlePlayerEncounter};

    WorldScene::Get()->OnPlayerEncounterFinished += {this, &WorldInterface::HandlePlayerDisengage};

    WorldScene::Get()->OnPlayerBattleStarted += {this, &WorldInterface::HandlePlayerBattle};

    WorldScene::Get()->OnPlayerBattleEnded += {this, &WorldInterface::HandleBattleEnded};

    WorldScene::Get()->OnSettlementFounded += {this, &WorldInterface::HandleSettlementFounded};

    controller = WorldController::Get();

    controller->onInventoryPressed += {this, &WorldInterface::HandleInventoryPressed};

    controller->onCharacterSelected += {this, &WorldInterface::HandleCharacterSelected};

    group::HumanMind::Get()->OnSellModeEntered += {this, &WorldInterface::HandleSellModeEntered};
}

void WorldInterface::Enable()
{
    InputHandler::RegisterEvent(CONSOLE_INPUT_KEY, {this, &WorldInterface::HandleConsoleToggled});

    InputHandler::RegisterEvent(MENU_CYCLE_INPUT_KEY, {this, &WorldInterface::HandleMenuCycled});

    canvas->Enable();

    HandleSettlementEntered();
}

void WorldInterface::Disable()
{
    InputHandler::UnregisterEvent(CONSOLE_INPUT_KEY);

    InputHandler::UnregisterEvent(MENU_CYCLE_INPUT_KEY);

    canvas->Disable();
}

void WorldInterface::HandleConsoleToggled()
{
    if(hoverInfo->IsLocallyActive() == true)
    {
        hoverInfo->Disable();
    }
    else
    {
        hoverInfo->Enable();
    }
}

void WorldInterface::HandleSellModeEntered()
{
    vendorCursor->Enable();
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

        if(reputationMenu->IsLocallyActive() == true)
        {
            reputationMenu->Disable();
        }

        if(questMenu->IsLocallyActive() == true)
        {
            questMenu->Disable();
        }
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

void WorldInterface::HandleMenuCycled()
{
    if(inventoryMenu->IsLocallyActive() == true)
    {
        inventoryMenu->Disable();

        reputationMenu->Enable();

        isInInventoryMode = false;
    }
    else if(reputationMenu->IsLocallyActive() == true)
    {
        questMenu->Enable();

        reputationMenu->Disable();
    }
    else if(questMenu->IsLocallyActive() == true)
    {
        inventoryMenu->Enable();

        questMenu->Disable();

        isInInventoryMode = true;
    }
    else
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
        auto settlement = playerGroup->GetCurrentSettlement();

        if(settlement != nullptr && settlement->IsPlayerControlled() == true)
        {
            if(settlementMenu->IsLocallyActive() == true)
            {
                settlementMenu->Disable();       

                productionMenu->Setup(settlement);

                productionMenu->Enable();
            }
            else if(productionMenu->IsLocallyActive() == true)
            {
                productionMenu->Disable();

                settlementMenu->Enable();
            }
        }
    }
}

void WorldInterface::HandleSpottingHovered()
{
    auto hoveredSpotting = group::HumanMind::Get()->GetHoveredSpotting();

    if(hoveredSpotting != nullptr)
    {
        groupHoverInfo->Setup(*hoveredSpotting);
        groupHoverInfo->Enable();
    }
    else
    {
        groupHoverInfo->Disable();
    }
}

void WorldInterface::HandleQuestStarted()
{
    questPopup->Setup(group::HumanMind::Get()->GetLastQuest(), true);

    questPopup->Enable();
}

void WorldInterface::HandleQuestFinished()
{
    questPopup->Setup(group::HumanMind::Get()->GetFinishedQuest(), false);

    questPopup->Enable();
}

void WorldInterface::HandleActionInitiated()
{
    auto timestamp = std::chrono::steady_clock::now();

    auto action = group::HumanMind::Get()->GetSelectedActionResult();

    if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupTimestamp).count() < TIME_BETWEEN_FADING_POPUPS)
    {
        *popupQueue.Grow() = {PopupTypes::ACTION, {action.ActionType, true}};
    }
    else
    {
        auto popup = *actionPopups.Add();

        popup->Setup(action.ActionType, true);

        popup->Enable();

        popupTimestamp = timestamp;
    }
}

void WorldInterface::HandleDiceRolled()
{
    auto timestamp = std::chrono::steady_clock::now();

    auto action = group::HumanMind::Get()->GetPerformedActionResult();

    if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupTimestamp).count() < TIME_BETWEEN_FADING_POPUPS)
    {
        *popupQueue.Grow() = {PopupTypes::ROLL, {action.Success}};
    }
    else
    {
        auto popup = *rollPopups.Add();

        popup->Setup(action.Success);

        popup->Enable();

        popupTimestamp = timestamp;
    }
}

void WorldInterface::HandlePlayerWealthChanged()
{
    auto timestamp = std::chrono::steady_clock::now();

    auto money = group::HumanMind::Get()->GetPlayerWealthChange();

    if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupTimestamp).count() < TIME_BETWEEN_FADING_POPUPS)
    {
        *popupQueue.Grow() = {PopupTypes::MONEY, {money}};
    }
    else
    {
        auto popup = *moneyPopups.Add();

        popup->Setup(money);

        popup->Enable();

        popupTimestamp = timestamp;
    }
}

void WorldInterface::HandlePlayerItemChanged()
{
    auto timestamp = std::chrono::steady_clock::now();

    auto itemChange = group::HumanMind::Get()->GetItemChange();

    if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupTimestamp).count() < TIME_BETWEEN_FADING_POPUPS)
    {
        *popupQueue.Grow() = {PopupTypes::ITEM, {itemChange}};
    }
    else
    {
        auto popup = *itemPopups.Add();

        popup->Setup(itemChange);

        popup->Enable();

        popupTimestamp = timestamp;
    }
}

void WorldInterface::HandleSettlementEntered()
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto currentSettlement = playerGroup->GetCurrentSettlement();

    if(currentSettlement == nullptr)
        return;

    settlementMenu->Setup(currentSettlement);
    settlementMenu->Enable();

    productionMenu->Disable();
}

void WorldInterface::HandleSettlementExited()
{
    settlementMenu->Disable();

    productionMenu->Disable();
}

void WorldInterface::HandlePlayerConquest()
{
    const auto settlement = WorldScene::Get()->GetConqueredSettlement();

    conquestPopup->Setup(settlement);

    conquestPopup->Enable();
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

    /*auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto currentSettlement = playerGroup->GetCurrentSettlement();
    if(currentSettlement == nullptr)
    {
        settlementMenu->Disable();
    }
    else
    {
        if(settlementMenu->IsLocallyActive() == false)
        {
            settlementMenu->Setup(currentSettlement);
            settlementMenu->Enable();
        }
    }*/

    if(popupQueue.IsEmpty() == false)
    {
        auto timestamp = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupTimestamp).count() > TIME_BETWEEN_FADING_POPUPS)
        {
            auto data = popupQueue.Pop();

            if(data->Type == PopupTypes::ROLL)
            {
                auto popup = *rollPopups.Add();

                popup->Setup(data->Data.RollData);

                popup->Enable();
            }
            else if(data->Type == PopupTypes::ACTION)
            {
                auto popup = *actionPopups.Add();

                popup->Setup(data->Data.ActionData.Type, data->Data.ActionData.HasStarted);

                popup->Enable();
            }
            else if(data->Type == PopupTypes::ITEM)
            {
                auto popup = *itemPopups.Add();

                popup->Setup(data->Data.ItemChange);

                popup->Enable();
            }
            else
            {
                auto popup = *moneyPopups.Add();

                popup->Setup(data->Data.MoneyData);

                popup->Enable();
            }

            popupTimestamp = timestamp;
        }
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

void WorldInterface::RemoveActionPopup(interface::ActionPopup *popup)
{
    actionPopups.Remove(popup);
}

void WorldInterface::RemoveRollPopup(interface::RollPopup *popup)
{
    rollPopups.Remove(popup);
}

void WorldInterface::RemoveMoneyPopup(interface::MoneyPopup *popup)
{
    moneyPopups.Remove(popup);
}

void WorldInterface::RemoveItemPopup(interface::ItemPopup *popup)
{
    itemPopups.Remove(popup);
}