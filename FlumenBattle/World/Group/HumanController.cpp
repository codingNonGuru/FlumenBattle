#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/Group/HumanController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

using namespace world::group;

static const SDL_Scancode searchInputKey = SDL_Scancode::SDL_SCANCODE_S;

static const SDL_Scancode takeShortRestInputKey = SDL_Scancode::SDL_SCANCODE_R;

static const SDL_Scancode travelInputKey = SDL_Scancode::SDL_SCANCODE_T;

HumanController::HumanController()
{
    OnActionPerformed = new Delegate();
}

void HumanController::DetermineAction(Group &group) const {}

void HumanController::PerformAction(Group &group) const
{
    group.PerformAction();

    OnActionPerformed->Invoke();
}

void HumanController::EnableInput()
{
    InputHandler::RegisterEvent(searchInputKey, {this, &HumanController::HandleSearch});

    InputHandler::RegisterEvent(takeShortRestInputKey, {this, &HumanController::HandleTakeShortRest});

    InputHandler::RegisterEvent(travelInputKey, {this, &HumanController::HandleTravel});
}

void HumanController::DisableInput()
{
    InputHandler::UnregisterEvent(searchInputKey, {this, &HumanController::HandleSearch});

    InputHandler::UnregisterEvent(takeShortRestInputKey, {this, &HumanController::HandleTakeShortRest});

    InputHandler::UnregisterEvent(travelInputKey, {this, &HumanController::HandleTravel});
}

void HumanController::HandleSearch()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::SEARCH))
    {
        playerGroup->SelectAction(world::GroupActions::SEARCH);
    }
}

void HumanController::HandleTakeShortRest()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TAKE_SHORT_REST))
    {
        playerGroup->SelectAction(world::GroupActions::TAKE_SHORT_REST);
    }
}

void HumanController::HandleTravel()
{
    auto tile = WorldController::Get()->GetHoveredTile();
    if(tile == nullptr)
        return;

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->ValidateAction(GroupActions::TRAVEL, {tile}))
    {
        playerGroup->SelectAction(world::GroupActions::TRAVEL, {tile});
    }
}