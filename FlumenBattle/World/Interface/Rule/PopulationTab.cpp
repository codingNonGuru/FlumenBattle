#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "PopulationTab.h"
#include "PopulationItem.h"
#include "FlumenBattle/World/Interface/Rule/RuleMenu.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Interface/CounterSet.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/WorldTime.h"
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
                Size(60, 100), 
                drawOrder_ + 1, 
                {popItemLayout}, 
                {"panel-border-015", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Disable();

        *popItems.Allocate() = item;
    }

    counterSet = ElementFactory::BuildElement <CounterSet>
    (
        {
            Size(), 
            drawOrder_, 
            {Position2(0.0f, -5.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    counterSet->Enable();

    growthBar = ElementFactory::BuildProgressBar <ProgressBar>
    (
        {Size(240, 28), drawOrder_ + 1, {Position2(0.0f, -5.0f), ElementAnchors::UPPER_CENTER, ElementPivots::LOWER_CENTER, counterSet}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    growthBar->Enable();

    growthDurationLabel = ElementFactory::BuildText(
        {drawOrder_ + 3, {growthBar}}, 
        {{"Small"}, Color::WHITE, "+5"}
    );
    growthDurationLabel->Enable();

    growthRateCounter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(20.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, growthBar}, {"WhiteDotBackdrop", false}}
    );
    growthRateCounter->Setup(&growthRate, Scale2(1.0f), "Medium");
    growthRateCounter->MakeSigned();
    growthRateCounter->Enable();

    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    ruleMenu->OnSettlementChanged += {this, &PopulationTab::HandleSettlementChanged};

    polity::HumanMind::Get()->OnPlayerSettlementPopIncrease += {this, &PopulationTab::UpdateItems};

    polity::HumanMind::Get()->OnPlayerSettlementPopDecrease += {this, &PopulationTab::UpdateItems};
}

void PopulationTab::HandleEnable() 
{
    UpdateItems();
}

void PopulationTab::HandleUpdate()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();
    
    auto settlement = ruleMenu->GetCurrentSettlement();

    growthBar->SetProgress(settlement->GetGrowthRatio());

    auto rate = settlement->GetPopulationHandler().GetGrowthRate();

    /*auto text = Word();
    if(rate > 0)
    {
        text = text << "+";
    }
    text = text << rate;

    growthDurationLabel->Setup(text);*/

    growthRate = rate;

    if(rate == 0)
    {
        growthDurationLabel->Setup(Word() << "-");
    }
    else
    {
        auto tickCount = settlement->GetPopulationHandler().GetTicksLeftUntilPopIncrease();
        tickCount /= WorldTime::HOUR_SIZE;

        if(tickCount <= 48)
            growthDurationLabel->Setup(Word() << tickCount << " hrs");
        else
        {
            tickCount /= 24;
            growthDurationLabel->Setup(Word() << tickCount << " days");
        }
    }
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
        (*item)->Setup(&cohort, settlement);
        (*item)->Enable();

        item++;
    }

    auto name = settlement->GetName();
    nameLabel->Setup(Word("<2>") << name.GetFirstCharacter() << "<1>" << (name.Get() + 1));

    counterSet->Setup(settlement->GetRaces());
}