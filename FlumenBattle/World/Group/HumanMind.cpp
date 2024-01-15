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
    playerGroup->travelActionData.IsOnRoute = true;

    playerGroup->travelActionData.PlannedDestinationCount = plannedPath.Tiles.GetSize() - 1;
    for(int i = 1; i < plannedPath.Tiles.GetSize(); ++i)
    {
        playerGroup->travelActionData.Route[i - 1] = *plannedPath.Tiles[i];
    }
    playerGroup->travelActionData.IsOnRoute = true;

    playerGroup->SelectAction(GroupActions::TRAVEL, {playerGroup->travelActionData.Route[0]});

    //if(playerGroup->ValidateAction(GroupActions::TRAVEL, {tile}))
    {
        //playerGroup->SelectAction(GroupActions::TRAVEL, {tile});
        //selectedActionResult = playerGroup->SelectAction(GroupActions::TRAVEL, {tile});

        //OnActionSelected->Invoke();
    }
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