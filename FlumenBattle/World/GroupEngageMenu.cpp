#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GroupEngageMenu.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/HumanMind.h"

using namespace world;

#define MAXIMUM_OPTION_COUNT 4

void GroupEngageMenu::HandleConfigure()
{
    descriptionLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 5.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "You come across a band of travellers most merry..."}
    );
    descriptionLabel->SetAlignment(Text::Alignments::LEFT);
    descriptionLabel->Enable();

    optionLabels.Initialize(MAXIMUM_OPTION_COUNT);

    auto startPosition = Position2(30.0f, 70.0f);

    for(int i = 0; i < MAXIMUM_OPTION_COUNT; ++i)
    {
        auto optionLabel = ElementFactory::BuildText(
            {Size(100, 100), drawOrder_ + 1, {startPosition, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
            {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "[F] Do battle with these treacherous arseholes"}
        );
        optionLabel->SetAlignment(Text::Alignments::LEFT);
        optionLabel->Disable();

        *optionLabels.Add() = optionLabel;

        startPosition.y += optionLabel->GetSize().y;
    }
}

struct OptionData
{
    SDL_Scancode Key;

    Event Event;

    Phrase Text;
};

void GroupEngageMenu::HandleEnable()
{
    *group::HumanMind::Get()->OnSkillCheckRolled += {this, &GroupEngageMenu::RefreshOptions};

    RefreshOptions();
}

void GroupEngageMenu::HandleDisable()
{
    *group::HumanMind::Get()->OnSkillCheckRolled -= {this, &GroupEngageMenu::RefreshOptions};

    DisableInput();
}

void GroupEngageMenu::HandleFightPressed()
{
    Disable();

    WorldScene::Get()->InitiatePlayerBattle();
}

void GroupEngageMenu::HandlePersuadePressed()
{
    WorldScene::Get()->InitiatePlayerPersuasion();

    //RefreshOptions();
}

void GroupEngageMenu::HandleLeavePressed()
{
    Disable();

    WorldScene::Get()->FinishPlayerEncounter();
}

void GroupEngageMenu::RefreshOptions()
{
    std::cout<<"refresh options\n";
    DisableInput();

    auto player = WorldScene::Get()->GetPlayerGroup();

    auto encounter = WorldController::Get()->GetPlayerBattle();

    if(encounter->HasBattleEnded() == true)
    {
        descriptionLabel->Setup("My liege, we have made short-shrift of their wretched kin...");
    }
    else
    {
        descriptionLabel->Setup("You come across a band of travellers most merry...");
    }

    for(auto label : optionLabels)
    {
        label->Disable();
    }

    auto label = optionLabels.GetStart();

    if(player->ValidateAction(group::GroupActions::FIGHT) == true)
    {
        (*label)->Enable();
        (*label)->Setup("[F] Do battle with these treacherous arseholes");
        label++;

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_F, {this, &GroupEngageMenu::HandleFightPressed});
    }

    if(player->ValidateAction(group::GroupActions::DISENGAGE) == true)
    {
        (*label)->Enable();
        (*label)->Setup("[L] Bid them a rather jolly farewell");
        label++;

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_L, {this, &GroupEngageMenu::HandleLeavePressed});
    }
    else if(player->ValidateAction(group::GroupActions::PERSUADE) == true)
    {
        (*label)->Enable();
        (*label)->Setup("[P] Attend their ears with sweetly honeyd words");
        label++;

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_P, {this, &GroupEngageMenu::HandlePersuadePressed});
    }
}

void GroupEngageMenu::DisableInput()
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_F, {this, &GroupEngageMenu::HandleFightPressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_L, {this, &GroupEngageMenu::HandleLeavePressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_P, {this, &GroupEngageMenu::HandlePersuadePressed});
}