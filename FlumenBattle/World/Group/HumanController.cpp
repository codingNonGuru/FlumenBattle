#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/World/Group/HumanController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::group;

static const SDL_Scancode searchInputKey = SDL_Scancode::SDL_SCANCODE_S;

static const SDL_Scancode takeShortRestInputKey = SDL_Scancode::SDL_SCANCODE_R;

static world::GroupActions selectedAction = world::GroupActions::NONE;

HumanController::HumanController()
{
    OnActionPerformed = new Delegate();
}

void HumanController::DetermineAction(Group &group) const
{
    if(selectedAction != world::GroupActions::NONE)
    {
        group.SelectAction(selectedAction);

        selectedAction = world::GroupActions::NONE;
    }
}

void HumanController::PerformAction(Group &group) const
{
    group.PerformAction();
    //group.SelectAction(selectedAction);

    OnActionPerformed->Invoke();
}

void HumanController::EnableInput()
{
    InputHandler::RegisterEvent(searchInputKey, this, &HumanController::HandleSearch);

    InputHandler::RegisterEvent(takeShortRestInputKey, this, &HumanController::HandleTakeShortRest);
}

void HumanController::DisableInput()
{
    InputHandler::UnregisterEvent(searchInputKey, this, &HumanController::HandleSearch);

    InputHandler::UnregisterEvent(takeShortRestInputKey, this, &HumanController::HandleTakeShortRest);
}

void HumanController::HandleSearch()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    auto searchAction = GroupActionFactory::Get()->BuildAction(world::GroupActions::SEARCH);

    auto canDo = searchAction->CanPerform(*playerGroup);
    if(canDo == false)
        return;

    selectedAction = world::GroupActions::SEARCH;

    //WorldScene::Get()->GetPlayerGroup()->SelectAction(GroupActions::SEARCH);

    //TaskManager::Add()->Initialize(this, &HumanController::StartBattle, 1.0f);

    //DisableInput();
}

void HumanController::HandleTakeShortRest()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    auto restAction = GroupActionFactory::Get()->BuildAction(world::GroupActions::TAKE_SHORT_REST);

    auto canDo = restAction->CanPerform(*playerGroup);
    if(canDo == false)
        return;

    selectedAction = world::GroupActions::TAKE_SHORT_REST;
    //auto canDo = GroupActionFactory::Get()->BuildAction(GroupActions::TAKE_SHORT_REST);
    //if(canDo == false)
        //return;

    //WorldScene::Get()->GetPlayerGroup()->SelectAction(GroupActions::TAKE_SHORT_REST);

    //TaskManager::Add()->Initialize(this, &HumanController::Rest, 1.0f);

    //DisableInput();
}