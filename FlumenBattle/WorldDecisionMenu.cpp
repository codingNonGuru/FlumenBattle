#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Delegate/Event.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/Utility/Utility.h"

#define ROLL_LABEL_COUNT 7

Array <Phrase> rollLabelTexts = Array <Phrase> (ROLL_LABEL_COUNT);

Array <Color> rollLabelColors = Array <Color> (ROLL_LABEL_COUNT);

Index currentRollLabelIndex = 0;

using namespace world;
using namespace group;

void WorldDecisionMenu::HandleConfigure() 
{
    rollLabels.Initialize(ROLL_LABEL_COUNT);

    for(Index i = 0; i < ROLL_LABEL_COUNT; ++i)
    {
        static auto defaultText = "";

        auto position = Position2(0.0f, 90.0f - (float)i * 25.0f);
        auto opacity = 0.4f + (float)i * 0.1f;
        auto rollLabel = ElementFactory::BuildText(
            {Size(size_.x - 10, 150), drawOrder_ + 1, {position, this}},
            {{"Small"}, Color::RED, defaultText}
        );
        rollLabel->SetAlignment(Text::Alignments::LEFT);
        rollLabel->SetOpacity(opacity);
        rollLabel->Enable();

        *rollLabels.Add() = rollLabel;

        *rollLabelTexts.Add() = Phrase(defaultText);
        *rollLabelColors.Add() = Color::RED;
    }
    /*travelLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 30.0f), this}},
        {{"Medium"}, Color::RED * 0.5f, "Press [T] to travel to location."}
    );
    travelLabel->Enable();

    searchLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"Medium"}, Color::RED * 0.5f, "Press [S] to search for enemies."}
    );
    searchLabel->Enable();

    restLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -30.0f), this}},
        {{"Medium"}, Color::RED * 0.5f, "Press [R] to rest."}
    );
    restLabel->Enable();*/

    statusLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -85.0f), this}},
        {{"Medium"}, Color::RED * 0.5f, "Current action: None"}
    );
    statusLabel->Enable();

    *group::HumanMind::Get()->OnActionSelected += {this, &WorldDecisionMenu::HandleActionSelected};

    *group::HumanMind::Get()->OnSkillCheckRolled += {this, &WorldDecisionMenu::HandleActionSelected};
}

void WorldDecisionMenu::HandleUpdate() 
{
    Phrase text = "Current action: ";
    
    auto group = WorldScene::Get()->GetPlayerGroup();
    if(group->GetAction())
    {
        switch(group->GetAction()->Type)
        {
            case GroupActions::TRAVEL:
                text << "Travelling " << 
                    [&] {
                    switch(group->travelActionData.Intensity)
                    {
                        case ActionIntensities::LEISURELY:
                            return "leisurely";
                        case ActionIntensities::NORMAL:
                            return "normally";
                        case ActionIntensities::INTENSE:
                            return "intensely";
                    }
                    } () << " to " << [&] {
                    switch(group->GetDestination()->Biome->Type)
                    {
                        case WorldBiomes::DESERT:
                            return "Desert";
                        case WorldBiomes::STEPPE:
                            return "Steppe";
                        case WorldBiomes::WOODS:
                            return "Woods";
                    }
                } ();
                break;
            case GroupActions::TAKE_LONG_REST:
                text << "Taking a long rest";
                break;
            case GroupActions::TAKE_SHORT_REST:
                text << "Taking a short rest";
                break;
            case GroupActions::SEARCH:
                text << "Searching for enemies";
                break;
            case GroupActions::FIGHT:
                text << "Fighting";
                break;
        }

        auto progressRate = group->GetProgressRate();
        int duration = group->GetRemainingActionDuration() / (6 * progressRate);
        
        if(group->GetRemainingActionDuration() % (6 * progressRate) > 2)
            duration += 1;

        if(duration < 0)
            duration = 0;

        text << " (" << duration << " hours left)";
    }
    else
    {
        text << "None";
    }

    statusLabel->Setup(text);
}

void WorldDecisionMenu::HandleActionSelected()
{
    auto result = group::HumanMind::Get()->GetSelectedActionResult();
    auto group = WorldScene::Get()->GetPlayerGroup();

    Phrase text = [&result] {
        switch(result.Success)
        {
            case utility::SuccessTypes::CRITICAL_SUCCESS:
                return "CRITICAL SUCCESS";
            case utility::SuccessTypes::SUCCESS:
                return "SUCCESS";
            case utility::SuccessTypes::FAILURE:
                return "FAILURE";
            case utility::SuccessTypes::CRITICAL_FAILURE:
                return "CRITICAL FAILURE";
        }
    } (); 
    
    text << " - You rolled a ";

    text << [&result] {
        switch(result.Skill)
        {
            case character::SkillTypes::SURVIVAL:
                return "Survival";
            case character::SkillTypes::PERCEPTION:
                return "Perception";
            case character::SkillTypes::PERSUASION:
                return "Persuasion";
            default:
                return "Skill";
        }
    } ();

    text << " check of ";

    text << result.Success.Roll + result.Success.Modifier << " (" << result.Success.Roll << "+" << result.Success.Modifier << ") against a DC of " << result.Success.DC;

    *rollLabelTexts.Get(currentRollLabelIndex) = text;
    *rollLabelColors.Get(currentRollLabelIndex++) = result.Success.IsAnySuccess() ? Color::DARK_GREEN : Color::RED;

    if(currentRollLabelIndex >= ROLL_LABEL_COUNT)
        currentRollLabelIndex = 0;

    for(int i = 0; i < ROLL_LABEL_COUNT; ++i)
    {
        auto label = *rollLabels.Get(i);

        auto textIndex = i + currentRollLabelIndex;
        if(textIndex >= ROLL_LABEL_COUNT)
        {
            textIndex -= ROLL_LABEL_COUNT;
        }
        auto text = rollLabelTexts.Get(textIndex);
        label->Setup(*text);

        auto color = rollLabelColors.Get(textIndex);
        label->SetColor(*color);
    }
}