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
#include "FlumenBattle/World/Interface/ConquestPopup.h"
#include "FlumenBattle/World/Interface/RecruitmentMenu.h"
#include "FlumenBattle/World/Interface/WorkerPlaceCursor.h"
#include "FlumenBattle/World/Interface/SideButtonSet.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Interface/TileResourceInfo.h"
#include "FlumenBattle/World/Interface/InstructionInfo.h"
#include "FlumenBattle/World/Interface/ExploreInfo.h"

using namespace world;

WorldController *controller = nullptr;

static const auto CONSOLE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_GRAVE;

static const auto MENU_CYCLE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_TAB;

#define ACTION_POPUP_CAPACITY 4

#define ROLL_POPUP_CAPACITY 16

auto popupTimestamp = std::chrono::steady_clock::now();

#define TIME_BETWEEN_FADING_POPUPS 300

#define SETTLEMENT_LABEL_ZOOM_LIMIT 1.5f

static auto majorCentralMenus = container::Array <Element *> (32);

WorldInterface::WorldInterface() : popupQueue(ROLL_POPUP_CAPACITY * 4)
{
    canvas = ElementFactory::BuildCanvas();
    canvas->SetInteractivity(true);

    decisionMenu = ElementFactory::BuildElement <WorldDecisionMenu>(
        {Size(1080, 220), DrawOrder(10), {Position2(0.0f, -5.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, canvas}, {false}, Opacity(0.75f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement <WorldInfoPanel>(
        {Size(canvas->GetWidth() - 10, 80), DrawOrder(6), {Position2(0.0f, 5.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, canvas}, {false}, Opacity(0.75f)}
    );
    infoPanel->Enable();

    hoverInfo = ElementFactory::BuildElement <WorldHoverInfo>(
        {Size(540, 420), DrawOrder(6), {canvas}, {false}, Opacity(0.5f)}
    );

    engageMenu = ElementFactory::BuildElement <GroupEngageMenu>(
        {Size(900, 360), DrawOrder(10), {Position2(0.0f, -5.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, canvas}, {false}, Opacity(0.75f)}
    );

    inventoryMenu = ElementFactory::BuildElement <interface::InventoryMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(12), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    *majorCentralMenus.Allocate() = inventoryMenu;

    reputationMenu = ElementFactory::BuildElement <interface::ReputationMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(12), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    *majorCentralMenus.Allocate() = reputationMenu;

    questMenu = ElementFactory::BuildElement <interface::QuestMenu>
    (
        {
            Size(480, 540), 
            DrawOrder(12), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    *majorCentralMenus.Allocate() = questMenu;

    settlementMenu = ElementFactory::BuildElement <interface::SettlementMenu>
    (
        {
            Size(320, 400), 
            DrawOrder(10), 
            {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );

    explorationMenu = ElementFactory::BuildElement <interface::ExplorationMenu>
    (
        {
            Size(320, 400), 
            DrawOrder(10), 
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
        {Size(210, 320), DrawOrder(10), {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nullptr}, {false}, Opacity(0.6f)}
    );

    travelBackdrop = ElementFactory::BuildElement <Element>
    (
        {Size(220, 50), DrawOrder(10), {Position2(0.0f, 200.0f), canvas}, {false}, Opacity(0.6f)}
    );

    travelLabel = ElementFactory::BuildText
    (
        {DrawOrder(11), {travelBackdrop}},
        {{"Medium"}, Color::RED * 0.5f}
    );
    travelLabel->Enable();

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

    workerPlaceCursor = ElementFactory::BuildElement <interface::WorkerPlaceCursor>
    (
        {
            DrawOrder(7), 
            {canvas}, 
            {"WorkHammer", false}
        }
    );
    workerPlaceCursor->FollowMouse();

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
            DrawOrder(10),
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, canvas},
            {false},
            Opacity(0.7f)
        }
    );

    questPopup = ElementFactory::BuildElement <interface::QuestPopup>
    (
        {
            Size(480, 200), 
            DrawOrder(10), 
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

    conquestPopup = ElementFactory::BuildElement <interface::ConquestPopup>
    (
        {
            Size(480, 200), 
            DrawOrder(10), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    conquestPopup->SetupBasicAnimations();

    recruitmentMenu = ElementFactory::BuildElement <interface::RecruitmentMenu>
    (
        {
            Size(660, 420), 
            DrawOrder(10), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    recruitmentMenu->SetupBasicAnimations();

    *majorCentralMenus.Allocate() = recruitmentMenu;

    ruleMenu = ElementFactory::BuildElement <interface::rule::RuleMenu>
    (
        {
            Size(720, 500), 
            DrawOrder(10), 
            {canvas}, 
            {false}, 
            Opacity(0.9f)
        }
    );
    ruleMenu->Setup();
    ruleMenu->SetupBasicAnimations();

    *majorCentralMenus.Allocate() = ruleMenu;

    sideButtonSet = ElementFactory::BuildElement <interface::SideButtonSet>
    (
        {
            DrawOrder(10),
            {ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, canvas}
        }
    );
    sideButtonSet->Setup(
        {
            {MajorMenus::INVENTORY, inventoryMenu}, 
            {MajorMenus::QUEST, questMenu}, 
            {MajorMenus::POLITY, ruleMenu}, 
            {MajorMenus::REPUTATION, reputationMenu}, 
            {MajorMenus::CAMP, nullptr}
        });
    sideButtonSet->Enable();

    static const auto MAXIMUM_ACTIVE_TILE_INFOS = [&] 
    {
        auto count = 1;

        for(int i = 0; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
        {
            count += i * 6;
        }

        return count;
    } ();

    tileResourceInfos.Initialize(MAXIMUM_ACTIVE_TILE_INFOS);

    for(int i = 0; i < tileResourceInfos.GetCapacity(); ++i)
    {
        auto info = ElementFactory::BuildElement <interface::TileResourceInfo>
        (
            {
                DrawOrder(5), 
                {canvas}
            }
        );
        info->Disable();

        *tileResourceInfos.Add() = info;
    }

    ElementFactory::BuildElement <interface::InstructionInfoSet> 
    (
        {
            DrawOrder(5), 
            {canvas}
        },
        64
    );

    ElementFactory::BuildElement <interface::ExploreInfoSet> 
    (
        {
            DrawOrder(5), 
            {canvas}
        },
        16
    );

    WorldController::Get()->OnResourceDisplayPressed += {this, &WorldInterface::HandleResourceDisplayPressed};

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

    controller->onRuleMenuPressed += {this, &WorldInterface::HandleRuleMenuPressed};

    group::HumanMind::Get()->OnSellModeEntered += {this, &WorldInterface::HandleSellModeEntered};

    WorldController::Get()->onWorkerPlaceModeEntered += {this, &WorldInterface::HandleWorkerPlaceModeEntered};
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

void WorldInterface::HandleWorkerPlaceModeEntered()
{
    workerPlaceCursor->Enable();
}

void WorldInterface::HandlePlayerEncounter()
{
    decisionMenu->Disable();

    engageMenu->Enable();
}

void WorldInterface::HandlePlayerDisengage()
{
    decisionMenu->Enable();
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
    if(isInRuleMode == true)
        return;

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

void WorldInterface::HandleResourceDisplayPressed()
{
    if(WorldController::Get()->ShouldDisplayResources() == false)
    {
        for(auto &info : tileResourceInfos)
        {
            info->Disable();
        }

        return;
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    auto playerTile = playerGroup->GetTile();

    auto nearbyTiles = playerTile->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE);

    auto info = tileResourceInfos.GetStart();
    for(auto &tile : nearbyTiles.Tiles)
    {
        if(tile->IsOwned() == false && playerSettlement->HasExplored(tile) == false)
            continue;

        (*info)->Setup(tile);
        (*info)->Enable();

        info++;
    }
}

void WorldInterface::HandleRuleMenuPressed()
{
    if(isInInventoryMode == true)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    const auto playerSettlement = playerGroup->GetCurrentSettlement();

    if(isInRuleMode == true)
    {
        ruleMenu->Disable();

        isInRuleMode = false;
    }
    else if(playerGroup->DoesRulePolity() == true)
    {
        ruleMenu->Enable();

        isInRuleMode = true;

        ruleMenu->SetCurrentSettlement(playerSettlement);
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

    if(action.ActionType == group::GroupActions::ENGAGE && action.Content.isAlreadyEngaged == true)
    {
        return;
    }

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
}

void WorldInterface::HandleSettlementExited()
{
    settlementMenu->Disable();
}

void WorldInterface::HandlePlayerConquest()
{
    const auto settlement = WorldScene::Get()->GetConqueredSettlement();

    conquestPopup->Setup(settlement);

    conquestPopup->Open();
}

void WorldInterface::Update()
{
    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    if(camera->GetZoomFactor() > SETTLEMENT_LABEL_ZOOM_LIMIT || WorldController::Get()->ShouldDisplayResources() == true)
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
        travelLabel->Setup("Plan your travel");
        travelBackdrop->Enable();
    }
    else if(controller->IsSettleModeActive() == true)
    {
        travelLabel->Setup("Settle mode");
        travelBackdrop->Enable();
    }
    else if(controller->IsBorderExpandActive() == true)
    {
        travelLabel->Setup("Grow your borders");
        travelBackdrop->Enable();
    }
    else if(controller->IsExploreModeActive() == true)
    {
        travelLabel->Setup("Map your environs");
        travelBackdrop->Enable();
    }
    else
    {
        travelBackdrop->Disable();
    }

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

void WorldInterface::OpenRecruitmentMenu()
{
    if(recruitmentMenu->IsLocallyActive() == true)
    {
        return;
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto currentSettlement = playerGroup->GetCurrentSettlement();

    recruitmentMenu->Setup(currentSettlement);

    recruitmentMenu->Open();
}

bool WorldInterface::IsAnyMajorMenuEnabled() const
{
    for(auto &menu : majorCentralMenus)
    {
        if(menu->IsGloballyActive() == true)
            return true;
    }

    return false;
}

bool WorldInterface::IsMajorMenu(Element *menu) const
{
    for(auto &majorMenu : majorCentralMenus)
    {
        if(menu == majorMenu)
            return true;
    }

    return false;
}