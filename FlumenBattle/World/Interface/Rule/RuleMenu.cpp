#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RuleMenu.h"
#include "EconomyTab.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Race.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto FADED_BUTTON_OPACITY = 0.5f;

void TabButton::HandleConfigure()
{
    label = ElementFactory::BuildText(
        {drawOrder_ + 2, {Position2(0.0f, 3.0f), this}}, 
        {{"Small"}, Color::WHITE, ""}
    );
    label->Enable();

    Fade();
}

void TabButton::Setup(RuleMenuTabs tab)
{
    auto text = [&tab] -> const char *
    {
        switch(tab)
        {
        case RuleMenuTabs::GENERAL:
            return "G";
        case RuleMenuTabs::ECONOMY:
            return "E";
        case RuleMenuTabs::TECHNOLOGY:
            return "T";
        }
    } ();

    label->Setup(text);
}

void TabButton::Light()
{
    label->SetOpacity(1.0f);
}

void TabButton::Fade()
{
    label->SetOpacity(FADED_BUTTON_OPACITY);
}

void RuleMenu::HandleUpdate()
{
    if(settlement == nullptr)
        return;

    nameLabel->Setup(settlement->GetName());

    auto text = Phrase() << settlement->GetPopulation() << " " << settlement->GetRace()->PluralName << " live in this jolly settlement, part of the realm of " << settlement->GetRuler()->GetName() << ".";
    populationLabel->Setup(text);

    text = Phrase() << settlement->GetName() << " has " << settlement->GetFreeWorkerCount() << " free workers.";
    workerLabel->Setup(text);
}

void RuleMenu::HandleConfigure()
{
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

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "Elric"}
    );
    nameLabel->Enable();

    populationLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nameLabel}}, 
        {{"Small"}, TEXT_COLOR, ""}
    );
    populationLabel->Enable();

    workerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, populationLabel}}, 
        {{"Small"}, TEXT_COLOR, ""}
    );
    workerLabel->Enable();

    buttonLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_RIGHT, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    buttonLayout->SetDistancing(1, 5.0f);
    buttonLayout->Enable();

    tabButtons.Initialize((int)RuleMenuTabs::COUNT);
    
    for(auto i = 0; i < (int)RuleMenuTabs::COUNT; ++i)
    {
        auto button = ElementFactory::BuildElement <TabButton>
        (
            { 
                drawOrder_ + 1, 
                {buttonLayout}, 
                {"FrameRound", false}
            }
        );
        button->Setup((RuleMenuTabs)i);
        button->AdjustSizeToTexture();
        button->Enable();

        *tabButtons.Add((RuleMenuTabs)i) = button;
    }
}

void RuleMenu::Setup()
{
    tabs.Initialize((int)RuleMenuTabs::COUNT);

    auto economyTab = ElementFactory::BuildElement <EconomyTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}, 
            Opacity(0.0f)
        }
    );

    *tabs.Add(RuleMenuTabs::ECONOMY) = economyTab;

    currentTab = RuleMenuTabs::ECONOMY;

    SetCurrentTab(RuleMenuTabs::ECONOMY);
}

void RuleMenu::SetCurrentSettlement(settlement::Settlement *settlement) 
{
    this->settlement = settlement;

    OnSettlementChanged.Invoke();
}

void RuleMenu::SetCurrentTab(RuleMenuTabs tab)
{
    (*tabs.Get(currentTab))->Disable();

    (*tabButtons.Get(currentTab))->Fade();

    currentTab = tab;

    (*tabs.Get(currentTab))->Enable();

    (*tabButtons.Get(currentTab))->Light();
}