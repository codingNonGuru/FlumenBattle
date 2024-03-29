#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "SettlementMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/SettlementMenuOption.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/ReputationHandler.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/WorldInterface.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto DEFAULT_FONT_SIZE = "Medium";

static constexpr auto OPTION_ITEM_SIZE = Size(280, 35);

void SettlementMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        {
            {DEFAULT_FONT_SIZE}, 
            TEXT_COLOR
        }
    );
    nameLabel->Enable();

    attitudeLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nameLabel}
        },
        {
            {"VerySmall"}, 
            TEXT_COLOR
        }
    );
    attitudeLabel->Enable();

    optionLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {Position2(10.0f, 80.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
    );
    optionLayout->SetDistancing(1, 5.0f);
    optionLayout->Enable();

    auto options = {
        SettlementMenuOptions::BUY_FOOD, 
        SettlementMenuOptions::BUY_MULE, 
        SettlementMenuOptions::SIGN_UP_TO_DELIVER_ITEM, 
        SettlementMenuOptions::FINISH_ITEM_DELIVERY,
        SettlementMenuOptions::RECRUIT_HEROES
        };

    for(auto &option : options)
    {
        auto optionItem = ElementFactory::BuildElement <SettlementMenuOption>
        (
            {
                OPTION_ITEM_SIZE, 
                drawOrder_ + 1, 
                {optionLayout}, 
                {"panel-border-001", true}
            }
        );
        optionItem->SetSpriteColor(BORDER_COLOR);
        optionItem->SetInteractivity(true);
        optionItem->Setup(this, option);
        optionItem->Enable();
    }
}

void SettlementMenu::HandleUpdate() 
{
    Phrase text;
    text << currentSettlement->GetName();
    nameLabel->Setup(text);

    text = currentSettlement->GetRace()->PluralName;
    text << ". ";

    static const auto &reputationHandler = group::HumanMind::Get()->GetPlayerReputation();
    const auto attitude = reputationHandler.GetAttitude(currentSettlement);

    text << utility::GetAttitudeName(attitude);
    text << " towards you.";

    attitudeLabel->Setup(text);
}

void SettlementMenu::Setup(settlement::Settlement *newSettlement)
{
    currentSettlement = newSettlement;
}

void SettlementMenu::ProcessOptionInput(SettlementMenuOptions option)
{
    switch(option)
    {
    case SettlementMenuOptions::BUY_FOOD:
        if(WorldController::Get()->CanBuyFood() == true)
        {
            WorldController::Get()->BuyFood();
        }
        break;
    case SettlementMenuOptions::BUY_MULE:
        if(WorldController::Get()->CanBuyMule() == true)
        {
            group::HumanMind::Get()->BuyMule();
        }
        break;
    case SettlementMenuOptions::SIGN_UP_TO_DELIVER_ITEM:
        {
            static const auto DELIVER_ITEM_QUEST_BASE_DURATION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::DELIVER_ITEM_QUEST_BASE_DURATION).Integer;

            auto destination = currentSettlement->GetLinks().GetRandom()->Other;
            group::HumanMind::Get()->AddQuest({group::QuestTypes::DELIVER_ITEM, currentSettlement, DELIVER_ITEM_QUEST_BASE_DURATION, {destination}});
            break;
        }
    case SettlementMenuOptions::FINISH_ITEM_DELIVERY:
        {
            group::HumanMind::Get()->FinishQuest(group::QuestTypes::DELIVER_ITEM, currentSettlement);
            break;
        }
    case SettlementMenuOptions::RECRUIT_HEROES:
        {
            WorldInterface::Get()->OpenRecruitmentMenu();
            break;
        }
    }
}