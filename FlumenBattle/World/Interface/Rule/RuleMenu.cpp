#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "RuleMenu.h"
#include "EconomyTab.h"
#include "TechTab.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto FADED_BUTTON_OPACITY = 0.5f;

static const auto ECONOMY_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_E, {InputHandler::SHIFT}};

static const auto TECHNOLOGY_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_K, {InputHandler::SHIFT}};

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
            return "K";
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

void RuleMenu::HandleEnable()
{
    InputHandler::RegisterEvent(ECONOMY_DISPLAY_KEY, {this, &RuleMenu::HandleEconomyPressed});

    InputHandler::RegisterEvent(TECHNOLOGY_DISPLAY_KEY, {this, &RuleMenu::HandleTechnologyPressed});
}

void RuleMenu::HandleDisable()
{
    InputHandler::UnregisterEvent(ECONOMY_DISPLAY_KEY);

    InputHandler::UnregisterEvent(TECHNOLOGY_DISPLAY_KEY);
}

void RuleMenu::HandleEconomyPressed()
{
    SetCurrentTab(RuleMenuTabs::ECONOMY);
}

void RuleMenu::HandleTechnologyPressed()
{
    SetCurrentTab(RuleMenuTabs::TECHNOLOGY);
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

    auto techTab = ElementFactory::BuildElement <TechTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}, 
            Opacity(0.0f)
        }
    );

    *tabs.Add(RuleMenuTabs::TECHNOLOGY) = techTab;

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