#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
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

#define MAXIMUM_OPTION_COUNT 5

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

void GroupEngageMenu::HandleConfigure()
{
    mainLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {Position2(20.0f, 20.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    mainLayout->SetDistancing(1, 15.0f);
    mainLayout->Enable();

    descriptionLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {mainLayout}},
        {{"Medium"}, TEXT_COLOR}
    );
    descriptionLabel->SetAlignment(Text::Alignments::LEFT);
    descriptionLabel->Enable();

    lootLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {mainLayout}},
        {{"Small"}, TEXT_COLOR}
    );
    lootLabel->SetAlignment(Text::Alignments::LEFT);

    resultLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {mainLayout}},
        {{"Small"}, TEXT_COLOR}
    );
    resultLabel->SetAlignment(Text::Alignments::LEFT);

    coinIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, lootLabel}, 
            {"Coin", false}
        }
    );
    coinIcon->AdjustSizeToTexture();
    coinIcon->UpdatePositionConstantly();
    coinIcon->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    optionLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {mainLayout}}
    );
    optionLayout->SetDistancing(1);
    optionLayout->SetOffset(Position2(20.0f, 0.0f));
    optionLayout->Enable();

    optionLabels.Initialize(MAXIMUM_OPTION_COUNT);

    for(int i = 0; i < MAXIMUM_OPTION_COUNT; ++i)
    {
        auto optionLabel = ElementFactory::BuildText(
            {drawOrder_ + 1, {optionLayout}},
            {{"Small"}, TEXT_COLOR}
        );
        optionLabel->SetAlignment(Text::Alignments::LEFT);
        optionLabel->Disable();

        *optionLabels.Add() = optionLabel;
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
    group::HumanMind::Get()->OnSkillCheckRolled += {this, &GroupEngageMenu::RefreshOptions};

    RefreshOptions();
}

void GroupEngageMenu::HandleDisable()
{
    group::HumanMind::Get()->OnSkillCheckRolled -= {this, &GroupEngageMenu::RefreshOptions};

    DisableInput();
}

void GroupEngageMenu::HandleFightPressed()
{
    Disable();

    WorldScene::Get()->InitiatePlayerBattle();
}

void GroupEngageMenu::HandleSneakPressed()
{
    WorldScene::Get()->InitiateDefenceBypass();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S);
}

void GroupEngageMenu::HandlePersuadePressed()
{
    WorldScene::Get()->InitiatePlayerPersuasion();
}

void GroupEngageMenu::HandleBribePressed()
{
    WorldScene::Get()->InitiateBribeGarrison();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}

void GroupEngageMenu::HandleLeavePressed()
{
    Disable();

    WorldScene::Get()->FinishPlayerEncounter();
}

void GroupEngageMenu::HandleConquerPressed()
{
    Disable();

    static const auto player = WorldScene::Get()->GetPlayerGroup();

    const auto encounter = WorldController::Get()->GetPlayerBattle();
    const auto enemy = encounter->GetOtherThan(player);

    WorldScene::Get()->AccomplishPlayerConquest(enemy->GetHome());

    WorldScene::Get()->FinishPlayerEncounter();
}

void GroupEngageMenu::RefreshOptions()
{
    DisableInput();

    static const auto player = WorldScene::Get()->GetPlayerGroup();

    const auto encounter = WorldController::Get()->GetPlayerBattle();
    const auto enemy = encounter->GetOtherThan(player);

    bool youAreLayingSiege = encounter->GetDefender() == enemy && enemy->GetCurrentSettlement() != nullptr && enemy->GetCurrentSettlement() == player->GetCurrentSettlement() && enemy->GetCurrentSettlement()->GetWallsLevel() != 0;

    if(encounter->HasBattleEnded() == true)
    {
        Phrase text("My liege, we have made short-shrift of their wretched kin.");
        //text << "Word will reach " << encounter->GetOtherThan(player)->GetHome()->GetName() << " that we are not to be trusted.";
        descriptionLabel->Setup(text);

        text = "You empty your foes pockets, bagging ";
        text << enemy->GetMoney();

        lootLabel->Setup(text);
        lootLabel->Enable();

        resultLabel->Disable();
    }
    else
    {
        Phrase text("You come across a ");
        text << utility::GetClassName(enemy->GetClass()) << " of " << enemy->GetHome()->GetName() << ".";

        if(youAreLayingSiege == true)
        {
            text << '\n';
            text << "They are protected by walls.";
        }

        descriptionLabel->Setup(text);

        lootLabel->Disable();

        static group::GroupActionResult sneakActionResult, bribeActionResult;

        auto &result = group::HumanMind::Get()->GetPerformedActionResult();
        if(result.ActionType == group::GroupActions::BRIBE_GARRISON)
        {
            bribeActionResult = result;
        }
        else if(result.ActionType == group::GroupActions::BYPASS_DEFENCES)
        {
            sneakActionResult = result;
        }

        text.Clear();

        if(player->HasAttemptedBypassingDefences())
        {   
            text << "Your attempt to bypass defences was a " << sneakActionResult.Success.GetString() << " (" << sneakActionResult.Success.Roll + sneakActionResult.Success.Modifier << ")";
        }

        if(player->HasAttemptedBribingGarrison())
        {
            if(player->HasAttemptedBypassingDefences())
            {
                text << "\n";
            }

            text << "Your attempt to bribe the garrison was a " << bribeActionResult.Success.GetString() << " (" << bribeActionResult.Success.Roll + bribeActionResult.Success.Modifier << ")";
        }

        if(player->HasAttemptedBypassingDefences() == false && player->HasAttemptedBribingGarrison() == false)
        {
            resultLabel->Disable();
        }
        else
        {
            resultLabel->Setup(text);
            resultLabel->Enable();
        }
    }

    for(auto label : optionLabels)
    {
        label->Disable();
    }

    auto label = optionLabels.GetStart();
    
    if(encounter->HasBattleEnded() == true && enemy->GetClass() == group::GroupClasses::GARRISON && enemy->GetCurrentSettlement()->IsDefended() == false)
    {
        (*label)->Enable();
        (*label)->Setup("[C] Conquer the settlement");
        label++;

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_C, {this, &GroupEngageMenu::HandleConquerPressed});
    }

    if(player->ValidateAction(group::GroupActions::FIGHT) == true)
    {
        (*label)->Enable();
        (*label)->Setup("[F] Do battle with these treacherous arseholes");
        label++;

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_F, {this, &GroupEngageMenu::HandleFightPressed});

        if(player->ValidateAction(group::GroupActions::BYPASS_DEFENCES) == true)
        {
            (*label)->Enable();
            auto text = Phrase("[S] Sneak to bypass their defences (DC ") << enemy->GetCurrentSettlement()->GetDefenceSneakDC() << ")";
            (*label)->Setup(text);
            label++;

            InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &GroupEngageMenu::HandleSneakPressed});
        }

        if(player->ValidateAction(group::GroupActions::BRIBE_GARRISON) == true)
        {
            (*label)->Enable();
            auto text = Phrase("[B] Pay to bribe their sentry (DC ") << enemy->GetCurrentSettlement()->GetBribeGarrisonDC() << ")";
            (*label)->Setup(text);
            label++;

            InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &GroupEngageMenu::HandleBribePressed});
        }
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
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_C);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}