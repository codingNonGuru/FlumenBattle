#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GroupEngageMenu.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world;

#define MAXIMUM_OPTION_COUNT 4

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

void GroupEngageMenu::HandleConfigure()
{
    descriptionLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(20.0f, 20.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"Large"}, TEXT_COLOR, "You come across a band of travellers most merry..."}
    );
    descriptionLabel->SetAlignment(Text::Alignments::LEFT);
    descriptionLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {Position2(), this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    optionLabels.Initialize(MAXIMUM_OPTION_COUNT);

    auto startPosition = Position2(45.0f, 85.0f);

    for(int i = 0; i < MAXIMUM_OPTION_COUNT; ++i)
    {
        auto optionLabel = ElementFactory::BuildText(
            {Size(100, 100), drawOrder_ + 1, {startPosition, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
            {{"Medium"}, Color::RED * 0.5f, "[F] Do battle with these treacherous arseholes"}
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
        Phrase text("My liege, we have made short-shrift of their wretched kin.\n");
        //text << "Word will reach " << encounter->GetOtherThan(player)->GetHome()->GetName() << " that we are not to be trusted.";
        descriptionLabel->Setup(text);
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
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_F);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_L);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_P);
}