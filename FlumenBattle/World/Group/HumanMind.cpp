#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

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
}

void HumanMind::DetermineAction(Group &group) const {}

void HumanMind::RegisterActionPerformance(Group &group, GroupActionResult result) const
{
    if(result.HasRolled == true)
    {
        selectedActionResult = result;
        
        OnActionPerformed->Invoke();
    }
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
    auto tile = WorldController::Get()->GetHoveredTile();
    if(tile == nullptr)
        return;

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TRAVEL, {tile}))
    {
        playerGroup->SelectAction(GroupActions::TRAVEL, {tile});
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