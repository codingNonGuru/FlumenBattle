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
#include "FlumenBattle/World/WorldController.h"

using namespace world::interface;

static constexpr auto OPTION_FONT_SIZE = "Small";

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void SettlementMenuOption::HandleConfigure() 
{
    label = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}
        },
        {
            {OPTION_FONT_SIZE}, 
            Color::RED * 0.5f
        }
    );
    label->Enable();

    priceCounter = ElementFactory::BuildElement <ResourceCounter> (
        {drawOrder_ + 1, {Position2(-60.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );
}

void SettlementMenuOption::Setup(SettlementMenu *newMenu, SettlementMenuOptions newOption) 
{
    menu = newMenu; 
    option = newOption;

    switch(option)
    {
    case SettlementMenuOptions::ATTACK:
        label->Setup("Attack");
        break;
    case SettlementMenuOptions::CONQUER:
        label->Setup("Conquer");
        break;
    case SettlementMenuOptions::LOOT:
        label->Setup("Loot");
        break;
    case SettlementMenuOptions::PILLAGE:
        label->Setup("Pillage");
        break;
    case SettlementMenuOptions::BUY_FOOD:
        priceCounter->Setup("Coin", std::function <int(void)> ([] -> int {
            auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

            return playerGroup->GetCurrentSettlement()->GetResourcePrice(settlement::ResourceTypes::FOOD);
        }));
        priceCounter->Enable();

        label->Setup("Buy food");
        break;
    case SettlementMenuOptions::BUY_MULE:
        priceCounter->Setup("Coin", std::function <int(void)> ([] -> int {
            auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

            return playerGroup->GetCurrentSettlement()->GetMulePrice();
        }));
        priceCounter->Enable();

        label->Setup("Buy mule");
        break;
    case SettlementMenuOptions::SIGN_UP_TO_DELIVER_ITEM:
        priceCounter->Disable();

        label->Setup("Sign up to deliver item");
        break;
    case SettlementMenuOptions::FINISH_ITEM_DELIVERY:
        priceCounter->Disable();

        label->Setup("Drop item");
        break;
    case SettlementMenuOptions::RECRUIT_HEROES:
        priceCounter->Disable();

        label->Setup("Recruit heroes");
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
    case SettlementMenuOptions::ATTACK:
        if(WorldController::Get()->CanAttackGarrison() == false)
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
    case SettlementMenuOptions::CONQUER:
        if(WorldController::Get()->CanConquerSettlement() == false)
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
    case SettlementMenuOptions::LOOT:
        if(WorldController::Get()->CanLootSettlement() == false)
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
    case SettlementMenuOptions::PILLAGE:
        if(WorldController::Get()->CanPillageSettlement() == false)
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
    case SettlementMenuOptions::BUY_FOOD:
        if(attitude == settlement::SettlementAttitudes::HOSTILE || attitude == settlement::SettlementAttitudes::UNFRIENDLY || WorldController::Get()->CanBuyFood() == false)
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
    case SettlementMenuOptions::BUY_MULE:
        if(attitude == settlement::SettlementAttitudes::HOSTILE || attitude == settlement::SettlementAttitudes::UNFRIENDLY || attitude == settlement::SettlementAttitudes::COLD || WorldController::Get()->CanBuyMule() == false)
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
    case SettlementMenuOptions::RECRUIT_HEROES:
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
    }
}