#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupActionData.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace world::group;

static const SDL_Scancode searchInputKey = SDL_Scancode::SDL_SCANCODE_S;

static const SDL_Scancode takeShortRestInputKey = SDL_Scancode::SDL_SCANCODE_R;

static const SDL_Scancode takeLongRestInputKey = SDL_Scancode::SDL_SCANCODE_L;

static const SDL_Scancode travelInputKey = SDL_Scancode::SDL_SCANCODE_T;

static const SDL_Scancode slackenActionKey = SDL_Scancode::SDL_SCANCODE_LEFTBRACKET;

static const SDL_Scancode intensifyActionKey = SDL_Scancode::SDL_SCANCODE_RIGHTBRACKET;

GroupActionResult selectedActionResult;

GroupActionResult performedActionResult;

utility::WorldPathData extendedPath;

int extendedPathIndex = 0;

HumanMind::HumanMind()
{
    OnActionSelected = new Delegate();

    OnActionPerformed = new Delegate();

    OnActionInitiated = new Delegate();

    OnSkillCheckRolled = new Delegate();
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

    InputHandler::RegisterEvent(takeShortRestInputKey, {this, &HumanMind::HandleTakeShortRest});

    InputHandler::RegisterEvent(takeLongRestInputKey, {this, &HumanMind::HandleTakeLongRest});

    InputHandler::RegisterEvent(travelInputKey, {this, &HumanMind::HandleTravel});

    InputHandler::RegisterEvent(slackenActionKey, {this, &HumanMind::HandleSlackenAction});

    InputHandler::RegisterEvent(intensifyActionKey, {this, &HumanMind::HandleIntensifyAction});
}

void HumanMind::DisableInput()
{
    InputHandler::UnregisterEvent(searchInputKey, {this, &HumanMind::HandleSearch});

    InputHandler::UnregisterEvent(takeShortRestInputKey, {this, &HumanMind::HandleTakeShortRest});

    InputHandler::UnregisterEvent(takeLongRestInputKey, {this, &HumanMind::HandleTakeLongRest});

    InputHandler::UnregisterEvent(travelInputKey, {this, &HumanMind::HandleTravel});

    InputHandler::UnregisterEvent(slackenActionKey, {this, &HumanMind::HandleSlackenAction});

    InputHandler::UnregisterEvent(intensifyActionKey, {this, &HumanMind::HandleIntensifyAction});
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
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::SEARCH))
    {
        playerGroup->SelectAction(GroupActions::SEARCH);
    }
}

void HumanMind::HandleTakeShortRest()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TAKE_SHORT_REST))
    {
        playerGroup->SelectAction(GroupActions::TAKE_SHORT_REST);
    }
}

void HumanMind::HandleTakeLongRest()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
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

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TRAVEL, {*plannedPath.Tiles[1]}) == false)
        return;

    extendedPath.Tiles.Clear();
    extendedPathIndex = 0;

    if(playerGroup->travelActionData.IsOnRoute && playerGroup->GetTravelProgress() < 0.5f)
    {
        playerGroup->CancelAction();
    }

    auto routeIndexDisplace = 0;
    if(playerGroup->travelActionData.IsOnRoute && playerGroup->GetTravelProgress() >= 0.5f)
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