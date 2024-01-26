#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Sound/SoundManager.h"

#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupActionData.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace world::group;

static const SDL_Scancode searchInputKey = SDL_Scancode::SDL_SCANCODE_S;

static const SDL_Scancode takeQuickRestInputKey = SDL_Scancode::SDL_SCANCODE_Q;

static const SDL_Scancode takeLongRestInputKey = SDL_Scancode::SDL_SCANCODE_L;

static const SDL_Scancode resumeTravelInputKey = SDL_Scancode::SDL_SCANCODE_R;

static const SDL_Scancode cancelTravelInputKey = SDL_Scancode::SDL_SCANCODE_C;

static const SDL_Scancode slackenActionKey = SDL_Scancode::SDL_SCANCODE_LEFTBRACKET;

static const SDL_Scancode intensifyActionKey = SDL_Scancode::SDL_SCANCODE_RIGHTBRACKET;

static const SDL_Scancode sellInputKey = SDL_Scancode::SDL_SCANCODE_V;

static const auto COIN_SOUNDS = container::Array {"Coin", "Coin2", "Coin3", "Coin4"};

static const auto DICE_ROLL_SOUND = "DiceRoll";

static const auto DICE_SOUND_INTERVAL = 1000;

static const auto VOLUME_PER_MARKET_ITEM = 10;

GroupActionResult selectedActionResult;

GroupActionResult performedActionResult;

utility::PathData <world::WorldTile> extendedPath;

int extendedPathIndex = 0;

auto diceSoundClock = steady_clock::now();

world::settlement::Settlement *previousSettlement = nullptr;

static container::Pool <GroupSpotting> groupSpottings;

static GroupSpotting *latestGroupSpotting = nullptr;

static container::Array <GroupSpotting *> latestFadings;

HumanMind::HumanMind()
{
    static const auto SPOTTING_COUNT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_SPOTTING_LIMIT).Integer;
    groupSpottings.Initialize(SPOTTING_COUNT);

    latestFadings.Initialize(SPOTTING_COUNT);

    OnActionSelected = new Delegate();

    OnActionPerformed = new Delegate();

    OnActionInitiated = new Delegate();

    OnSkillCheckRolled = new Delegate();

    OnItemAdded = new Delegate();

    OnSellModeEntered = new Delegate();

    OnSettlementEntered = new Delegate();

    OnSettlementExited = new Delegate();

    OnGroupSpotted = new Delegate();

    OnGroupFaded = new Delegate();

    *WorldScene::Get()->OnUpdateStarted += {this, &HumanMind::HandleSceneUpdate};
}

void HumanMind::DetermineAction(Group &group) const 
{
    if(group.travelActionData.IsOnRoute)
    {    
        if(group.GetDestination() == nullptr)
        {
            group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});

            if(extendedPath.Tiles.GetSize() != 0)
            {
                group.travelActionData.Route[TILES_PER_GROUP_ROUTE - 1] = *extendedPath.Tiles[extendedPathIndex];
                group.travelActionData.PlannedDestinationCount++;
                extendedPathIndex++;
                if(extendedPathIndex == extendedPath.Tiles.GetSize())
                {
                    extendedPath.Tiles.Clear();
                }
            }
        }
    }
    else
    {
    }
}

void HumanMind::RegisterActionPerformance(Group &group, GroupActionResult result) const
{
    selectedActionResult = result;

    if(result.HasRolled == true)
    {
        auto clock = std::chrono::steady_clock::now();
        auto timeSinceLastDiceSound = std::chrono::duration_cast<milliseconds>(clock - diceSoundClock);
        if(timeSinceLastDiceSound > std::chrono::milliseconds(DICE_SOUND_INTERVAL))
        {
            engine::SoundManager::Get()->PlaySound(DICE_ROLL_SOUND);
            diceSoundClock = clock;
        }

        if(group.IsDoing(GroupActions::SEARCH) == true)
        {
            static auto &worldTime = WorldScene::Get()->GetTime();

            auto spottedGroup = result.Content.spottedGroup;
            auto existingSpotting = groupSpottings.Find(spottedGroup->GetUniqueId());

            if(existingSpotting == nullptr)
            {
                auto newSpotting = groupSpottings.Add();
                *newSpotting = 
                {
                    spottedGroup, 
                    spottedGroup->GetUniqueId(),
                    spottedGroup->GetClass(),
                    spottedGroup->GetLeader()->GetName(),
                    spottedGroup->GetHome()->GetName(),
                    worldTime.TotalHourCount, 
                    result.Success.IsCriticalSuccess(), 
                    group.GetTile()->GetDistanceTo(*spottedGroup->GetTile()),
                    spottedGroup->GetVisualPosition()
                };

                latestGroupSpotting = newSpotting;
            }
            else
            {
                *existingSpotting = 
                {
                    spottedGroup, 
                    spottedGroup->GetUniqueId(),
                    spottedGroup->GetClass(),
                    spottedGroup->GetLeader()->GetName(),
                    spottedGroup->GetHome()->GetName(),
                    worldTime.TotalHourCount, 
                    result.Success.IsCriticalSuccess(), 
                    group.GetTile()->GetDistanceTo(*spottedGroup->GetTile()),
                    spottedGroup->GetVisualPosition()
                };

                latestGroupSpotting = existingSpotting;                
            }

            OnGroupSpotted->Invoke();
        }

        OnSkillCheckRolled->Invoke();
    }

    OnActionPerformed->Invoke();
}

void HumanMind::RegisterActionInitiation(Group &, GroupActionResult) const 
{
    OnActionInitiated->Invoke();
}

void HumanMind::EnableInput()
{
    InputHandler::RegisterEvent(searchInputKey, {this, &HumanMind::HandleSearch});

    InputHandler::RegisterEvent(takeQuickRestInputKey, {this, &HumanMind::HandleTakeQuickRest});

    InputHandler::RegisterEvent(takeLongRestInputKey, {this, &HumanMind::HandleTakeLongRest});

    InputHandler::RegisterEvent(resumeTravelInputKey, {this, &HumanMind::HandleResumeTravel});

    InputHandler::RegisterEvent(cancelTravelInputKey, {this, &HumanMind::HandleCancelTravel});

    InputHandler::RegisterEvent(slackenActionKey, {this, &HumanMind::HandleSlackenAction});

    InputHandler::RegisterEvent(intensifyActionKey, {this, &HumanMind::HandleIntensifyAction});

    InputHandler::RegisterContinualEvent(sellInputKey, {this, &HumanMind::HandleSellModeEntered}, {this, &HumanMind::HandleSellModeExited});

    auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleTravel};
}

void HumanMind::DisableInput()
{
    InputHandler::UnregisterEvent(searchInputKey);

    InputHandler::UnregisterEvent(takeQuickRestInputKey);

    InputHandler::UnregisterEvent(takeLongRestInputKey);

    InputHandler::UnregisterEvent(resumeTravelInputKey);

    InputHandler::UnregisterEvent(cancelTravelInputKey);

    InputHandler::UnregisterEvent(slackenActionKey);

    InputHandler::UnregisterEvent(intensifyActionKey);

    InputHandler::UnregisterContinualEvent(sellInputKey);

    auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleTravel};
}

void HumanMind::HandleSceneUpdate()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->GetCurrentSettlement() != previousSettlement)
    {
        if(playerGroup->GetCurrentSettlement() != nullptr)
        {
            OnSettlementEntered->Invoke();
        }
        else
        {
            OnSettlementExited->Invoke();
        }
    }

    previousSettlement = playerGroup->GetCurrentSettlement();

    UpdateSpottings();
}

void HumanMind::UpdateSpottings()
{
    static const auto maximumLifetime = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_SPOTTING_LIFETIME).Integer;

    latestFadings.Reset();

    static auto &worldTime = WorldScene::Get()->GetTime();
    for(auto &spotting : groupSpottings)
    {
        auto hoursElapsed = worldTime.TotalHourCount - spotting.TimeInHours;
        if(hoursElapsed >= maximumLifetime)
        {
            *latestFadings.Add() = &spotting;
        }
    }

    for(auto &spotting : latestFadings)
    {
        groupSpottings.RemoveAt(spotting);
    }

    if(latestFadings.GetSize() > 0)
    {
        OnGroupFaded->Invoke();
    }
}

const GroupActionResult & HumanMind::GetSelectedActionResult()
{
    return selectedActionResult;
}

const GroupActionResult & HumanMind::GetPerformedActionResult()
{
    return performedActionResult;
}

void HumanMind::HandleSearch()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::SEARCH))
    {
        playerGroup->SelectAction(GroupActions::SEARCH);
    }
}

void HumanMind::HandleTakeQuickRest()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TAKE_SHORT_REST))
    {
        playerGroup->SelectAction(GroupActions::TAKE_SHORT_REST);
    }
}

void HumanMind::HandleTakeLongRest()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TAKE_LONG_REST))
    {
        playerGroup->SelectAction(GroupActions::TAKE_LONG_REST);
    }
}

void HumanMind::HandleTravel()
{
    auto plannedPath = WorldController::Get()->GetPlannedPath();
    if(plannedPath.Tiles.GetSize() == 0)
        return;

    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TRAVEL, {*plannedPath.Tiles[1]}) == false)
        return;

    extendedPath.Tiles.Clear();
    extendedPathIndex = 0;

    bool isHalfwayThrough = playerGroup->GetTravelProgress() >= 0.5f;
    if(playerGroup->travelActionData.IsOnRoute && isHalfwayThrough == false)
    {
        playerGroup->CancelAction();
    }

    auto routeIndexDisplace = 0;
    if(playerGroup->travelActionData.IsOnRoute && isHalfwayThrough == true)
    {
        routeIndexDisplace = 1;
    }

    playerGroup->travelActionData.IsOnRoute = true;

    auto pathSize = routeIndexDisplace + plannedPath.Tiles.GetSize() - 1;
    playerGroup->travelActionData.PlannedDestinationCount = pathSize <= TILES_PER_GROUP_ROUTE ? pathSize : TILES_PER_GROUP_ROUTE;
    for(int i = 1; i < (plannedPath.Tiles.GetSize() <= TILES_PER_GROUP_ROUTE + 1 ? plannedPath.Tiles.GetSize() : TILES_PER_GROUP_ROUTE + 1); ++i)
    {
        auto index = routeIndexDisplace + i - 1;
        if(index >= TILES_PER_GROUP_ROUTE)
            continue;

        playerGroup->travelActionData.Route[index] = *plannedPath.Tiles[i];
    }
    playerGroup->travelActionData.IsOnRoute = true;

    if(pathSize > TILES_PER_GROUP_ROUTE)
    {
        for(int i = TILES_PER_GROUP_ROUTE + 1; i < plannedPath.Tiles.GetSize() + routeIndexDisplace; ++i)
        {
            *extendedPath.Tiles.Add() = *plannedPath.Tiles[i - routeIndexDisplace];
        }
        extendedPathIndex = 0;
    }

    playerGroup->SelectAction(GroupActions::TRAVEL, {playerGroup->travelActionData.Route[0]});
}

void HumanMind::HandleResumeTravel()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    if(playerGroup->travelActionData.IsOnRoute == false)
        return;

    playerGroup->SelectAction(GroupActions::TRAVEL, {playerGroup->travelActionData.Route[0]});
}

void HumanMind::HandleCancelTravel()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    auto isHalfwayThrough = playerGroup->GetTravelProgress() >= 0.5f;
    if(playerGroup->travelActionData.IsOnRoute && isHalfwayThrough == true)
        return;

    if(playerGroup->IsDoing(GroupActions::TRAVEL) == true)
    {
        playerGroup->CancelAction();
    }

    playerGroup->travelActionData.IsOnRoute = false;
    playerGroup->travelActionData.Progress = 0;
    playerGroup->travelActionData.PlannedDestinationCount = 0;

    extendedPath.Tiles.Clear();
    extendedPathIndex = 0;
}

void HumanMind::HandleSlackenAction()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    playerGroup->SlackenAction();
}

void HumanMind::HandleIntensifyAction()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    playerGroup->IntensifyAction();
}

void HumanMind::HandleSellModeEntered()
{   
    if(WorldInterface::Get()->IsInInventoryMode() == false)
        return;

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->GetCurrentSettlement() == nullptr)
        return;

    isSellModeActive = true;

    OnSellModeEntered->Invoke();
}

void HumanMind::HandleSellModeExited()
{
    isSellModeActive = false;
}

void HumanMind::BuyFood()
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    auto playerSettlement = playerGroup->GetCurrentSettlement();

    auto foodResource = playerSettlement->GetResource(settlement::ResourceTypes::FOOD);
    auto foodPrice = foodResource->Type->Value;

    playerGroup->money -= foodPrice;
    playerGroup->AddItem(character::ItemTypes::FOOD, VOLUME_PER_MARKET_ITEM);

    auto sound = COIN_SOUNDS.GetRandom();
    engine::SoundManager::Get()->PlaySound(*sound);

    OnItemAdded->Invoke();
}

void HumanMind::SellItem(character::Item *item)
{
    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    playerGroup->RemoveItem(item);

    playerGroup->money += item->Type->Value * item->Amount;

    auto sound = COIN_SOUNDS.GetRandom();
    engine::SoundManager::Get()->PlaySound(*sound);
}

void HumanMind::PursueSighting(const GroupSpotting &spotting)
{  
    auto group = WorldScene::Get()->GetGroup(spotting.GroupUniqueId);
    if(group == nullptr)
        return;

    if(group->IsAlive() == false)
        return;

    if(group->IsDoing(GroupActions::ENGAGE) || group->IsDoing(GroupActions::FIGHT))
        return;

    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    WorldScene::Get()->InitiateEncounter(group, playerGroup);
}

const utility::PathData <world::WorldTile> HumanMind::GetFullPathData()
{
    utility::PathData <world::WorldTile> pathData;

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    for(int i = 0; i < playerGroup->travelActionData.PlannedDestinationCount; ++i)
    {
        *pathData.Tiles.Add() = playerGroup->travelActionData.Route[i];
    }

    if(extendedPath.Tiles.GetSize() == 0)
        return pathData;

    for(int i = extendedPathIndex; i < extendedPath.Tiles.GetSize(); ++i)
    {
        *pathData.Tiles.Add() = *extendedPath.Tiles[i];
    }

    return pathData;
}

world::WorldTile *HumanMind::GetFinalDestination() const
{
    if(extendedPath.Tiles.GetSize() == 0)
    {
        auto playerGroup = WorldScene::Get()->GetPlayerGroup();
        return playerGroup->GetFinalDestination();
    }
    else
    {
        return *(extendedPath.Tiles.GetLast() - 1);
    }
}

const GroupSpotting &HumanMind::GetLatestGroupSpotting() const
{
    return *latestGroupSpotting;
}

const container::Array <GroupSpotting *> &HumanMind::GetLatestFadings() const
{
    return latestFadings;
}

const container::Pool <GroupSpotting> &HumanMind::GetGroupSightings() const
{
    return groupSpottings;
}