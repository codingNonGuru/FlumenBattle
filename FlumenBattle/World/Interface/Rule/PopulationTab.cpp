#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "PopulationTab.h"
#include "PopulationItem.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/Config.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

void PopulationTab::HandleConfigure() 
{
    nameLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, HIGHLIGHT_COLOR, "Noonkelp"}
    );
    nameLabel->Enable();

    static const auto MAX_SETTLEMENT_POPULATION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAX_SETTLEMENT_POPULATION).Integer;

    popItems.Initialize(MAX_SETTLEMENT_POPULATION);

    popItemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_,
            {Position2{0.0f, 40.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        }
    );
    popItemLayout->SetDistancing(MAX_SETTLEMENT_POPULATION / 4, 5.0f);
    popItemLayout->AlignToCenter();
    popItemLayout->Enable();

    for(auto i = 0; i < MAX_SETTLEMENT_POPULATION; ++i)
    {
        auto item = ElementFactory::BuildElement <PopulationItem>
        (
            {
                Size(60, 85), 
                drawOrder_ + 1, 
                {popItemLayout}, 
                {"panel-border-015", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Disable();

        *popItems.Allocate() = item;
    }

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &PopulationTab::HandleSettlementChanged};
}

void PopulationTab::HandleEnable() 
{
    UpdateItems();
}

void PopulationTab::HandleSettlementChanged()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();
    if(settlement == nullptr)
    {
        nameLabel->Disable();

        return;
    }

    UpdateItems();

    nameLabel->Enable();
}

void PopulationTab::UpdateItems()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();

    auto &cohorts = settlement->GetPopCohorts();

    for(auto &item : popItems)
    {
        item->Disable();
    }

    auto item = popItems.GetStart();
    for(auto &cohort : cohorts)
    {
        (*item)->Setup(&cohort);
        (*item)->Enable();

        item++;
    }

    auto name = settlement->GetName();
    nameLabel->Setup(Word("<2>") << name.GetFirstCharacter() << "<1>" << (name.Get() + 1));
}