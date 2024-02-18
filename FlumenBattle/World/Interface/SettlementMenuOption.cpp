#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SettlementMenuOption.h"
#include "FlumenBattle/World/Interface/SettlementMenu.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/ReputationHandler.h"
#include "FlumenBattle/World/Group/Quest.h"

using namespace world::interface;

static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

static constexpr auto OPTION_FONT_SIZE = "Small";

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void SettlementMenuOption::HandleConfigure() 
{
    label = ElementFactory::BuildText
    (
        {
            Size(), 
            drawOrder_ + 1, 
            {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}
        },
        {
            {DEFAULT_FONT_TYPE, OPTION_FONT_SIZE}, 
            Color::RED * 0.5f, 
            ""
        }
    );
    label->Enable();

    priceCounter = ElementFactory::BuildElement <ResourceCounter> (
        {Size(), drawOrder_ + 1, {Position2(-60.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );
}

void SettlementMenuOption::Setup(SettlementMenu *newMenu, SettlementMenuOptions newOption) 
{
    menu = newMenu; 
    option = newOption;

    switch(option)
    {
    case SettlementMenuOptions::BUY_FOOD:
        priceCounter->Setup("Coin", std::function <int(void)> ([] -> int {
            auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

            return playerGroup->GetCurrentSettlement()->GetResourcePrice(settlement::ResourceTypes::FOOD);
        }));
        priceCounter->Enable();

        label->Setup("Buy food");
        break;
    case SettlementMenuOptions::SIGN_UP_TO_DELIVER_ITEM:
        priceCounter->Disable();

        label->Setup("Sign up to deliver item");
        break;
    case SettlementMenuOptions::FINISH_ITEM_DELIVERY:
        priceCounter->Disable();

        label->Setup("Drop item");
        break;
    }
}

void SettlementMenuOption::HandleLeftClick()
{
    menu->ProcessOptionInput(option);
}

void SettlementMenuOption::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(HOVERED_OPTION_OPACITY);
    }
    else
    {
        SetOpacity(BASE_OPTION_OPACITY);
    }

    static const auto &reputationHandler = group::HumanMind::Get()->GetPlayerReputation();
    const auto attitude = reputationHandler.GetAttitude(menu->GetCurrentSettlement());

    switch(option)
    {
    case SettlementMenuOptions::BUY_FOOD:
        if(attitude == settlement::SettlementAttitudes::HOSTILE || attitude == settlement::SettlementAttitudes::UNFRIENDLY)
        {
            SetInteractivity(false);

            SetOpacity(BASE_OPTION_OPACITY);

            label->SetOpacity(BASE_OPTION_OPACITY);
        }
        else
        {
            SetInteractivity(true);

            label->SetOpacity(HOVERED_OPTION_OPACITY);
        }
        break;
    case SettlementMenuOptions::SIGN_UP_TO_DELIVER_ITEM:
        {
            bool hasQuest = false;
            for(auto &quest : group::HumanMind::Get()->GetQuests())
            {
                if(quest.Origin == menu->GetCurrentSettlement() && quest.Type == group::QuestTypes::DELIVER_ITEM)
                {
                    hasQuest = true;
                    break;
                }
            }

            if(hasQuest == false && menu->GetCurrentSettlement()->GetLinks().GetSize() > 0)
            {
                SetInteractivity(true);

                label->SetOpacity(HOVERED_OPTION_OPACITY);
            }
            else
            {
                SetInteractivity(false);

                SetOpacity(BASE_OPTION_OPACITY);

                label->SetOpacity(BASE_OPTION_OPACITY);
            }
            break;
        }
    case SettlementMenuOptions::FINISH_ITEM_DELIVERY:
        {
            bool isTarget = false;
            for(auto &quest : group::HumanMind::Get()->GetQuests())
            {
                if(quest.Data.TargetSettlement == menu->GetCurrentSettlement() && quest.Type == group::QuestTypes::DELIVER_ITEM)
                {
                    isTarget = true;
                    break;
                }
            }

            if(isTarget == true)
            {
                SetInteractivity(true);

                label->SetOpacity(HOVERED_OPTION_OPACITY);
            }
            else
            {
                SetInteractivity(false);

                SetOpacity(BASE_OPTION_OPACITY);

                label->SetOpacity(BASE_OPTION_OPACITY);
            }
            break;
        }
    }
}