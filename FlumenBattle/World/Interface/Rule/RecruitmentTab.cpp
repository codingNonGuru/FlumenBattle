#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "RecruitmentTab.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/Rule/BuildingHoverInfo.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Interface/Rule/ProductionDecisionInterface.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_BUILDING_COUNT 16

void RecruitmentTab::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    decisionInterface = ElementFactory::BuildElement <interface::rule::ProductionDecisionInterface>
    (
        {
            Size(320, 400), 
            drawOrder_, 
            {Position2(0.0f, 30.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {false}, 
            Opacity(0.0f)
        },
        new ProductionInterfaceData{settlement::ProductionClasses::RECRUITMENT}
    );
    decisionInterface->Enable();

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &RecruitmentTab::HandleSettlementChanged};
}

void RecruitmentTab::HandleUpdate()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    auto settlement = ruleMenu->GetCurrentSettlement();

    if(settlement == nullptr)
        return;

    nameLabel->Setup(settlement->GetName());
}

void RecruitmentTab::HandleSettlementChanged() 
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        nameLabel->Disable();

        decisionInterface->Disable();

        return;
    }

    decisionInterface->Setup(settlement);

    decisionInterface->Enable();

    nameLabel->Enable();
}