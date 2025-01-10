#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "RuleMenu.h"
#include "EconomyTab.h"
#include "TechTab.h"
#include "RealmTab.h"
#include "BuildingTab.h"
#include "TradeTab.h"
#include "GroupTab.h"
#include "DiplomacyTab.h"
#include "RecruitmentTab.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto FADED_BUTTON_OPACITY = 0.5f;

static const auto ECONOMY_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_E, {InputHandler::SHIFT}};

static const auto TECHNOLOGY_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_K, {InputHandler::SHIFT}};

static const auto REALM_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_R, {InputHandler::SHIFT}};

static const auto BUILDING_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_B, {InputHandler::SHIFT}};

static const auto TRADE_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_T, {InputHandler::SHIFT}};

static const auto GROUPS_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_G, {InputHandler::SHIFT}};

static const auto DIPLOMACY_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_D, {InputHandler::SHIFT}};

static const auto RECRUITMENT_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_C, {InputHandler::SHIFT}};

void TabButton::HandleConfigure()
{
    label = ElementFactory::BuildText(
        {drawOrder_ + 2, {Position2(0.0f, 3.0f), this}}, 
        {{"Small"}, Color::WHITE}
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
        case RuleMenuTabs::REALM:
            return "R";
        case RuleMenuTabs::ECONOMY:
            return "E";
        case RuleMenuTabs::TECHNOLOGY:
            return "K";
        case RuleMenuTabs::BUILDING:
            return "B";
        case RuleMenuTabs::TRADE:
            return "T";
        case RuleMenuTabs::GROUPS:
            return "G";
        case RuleMenuTabs::DIPLOMACY:
            return "D";
        case RuleMenuTabs::RECRUITMENT:
            return "C";
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
    rulerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, 10.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    rulerLabel->Enable();
     
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

    InputHandler::RegisterEvent(REALM_DISPLAY_KEY, {this, &RuleMenu::HandleRealmPressed});

    InputHandler::RegisterEvent(BUILDING_DISPLAY_KEY, {this, &RuleMenu::HandleBuildingPressed});

    InputHandler::RegisterEvent(TRADE_DISPLAY_KEY, {this, &RuleMenu::HandleTradePressed});

    InputHandler::RegisterEvent(GROUPS_DISPLAY_KEY, {this, &RuleMenu::HandleGroupsPressed});

    InputHandler::RegisterEvent(DIPLOMACY_DISPLAY_KEY, {this, &RuleMenu::HandleDiplomacyPressed});

    InputHandler::RegisterEvent(RECRUITMENT_DISPLAY_KEY, {this, &RuleMenu::HandleRecruitmentPressed});

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    rulerLabel->Setup(LongWord() << "Realm of " << playerPolity->GetRuler()->GetName());
}

void RuleMenu::HandleDisable()
{
    InputHandler::UnregisterEvent(ECONOMY_DISPLAY_KEY);

    InputHandler::UnregisterEvent(TECHNOLOGY_DISPLAY_KEY);

    InputHandler::UnregisterEvent(REALM_DISPLAY_KEY);

    InputHandler::UnregisterEvent(BUILDING_DISPLAY_KEY);

    InputHandler::UnregisterEvent(TRADE_DISPLAY_KEY);

    InputHandler::UnregisterEvent(GROUPS_DISPLAY_KEY);

    InputHandler::UnregisterEvent(DIPLOMACY_DISPLAY_KEY);

    InputHandler::UnregisterEvent(RECRUITMENT_DISPLAY_KEY);
}

void RuleMenu::HandleEconomyPressed()
{
    SetCurrentTab(RuleMenuTabs::ECONOMY);
}

void RuleMenu::HandleTechnologyPressed()
{
    SetCurrentTab(RuleMenuTabs::TECHNOLOGY);
}

void RuleMenu::HandleRealmPressed()
{
    SetCurrentTab(RuleMenuTabs::REALM);
}

void RuleMenu::HandleBuildingPressed()
{
    SetCurrentTab(RuleMenuTabs::BUILDING);
}

void RuleMenu::HandleTradePressed()
{
    SetCurrentTab(RuleMenuTabs::TRADE);
}

void RuleMenu::HandleGroupsPressed()
{
    SetCurrentTab(RuleMenuTabs::GROUPS);
}

void RuleMenu::HandleDiplomacyPressed()
{
    SetCurrentTab(RuleMenuTabs::DIPLOMACY);
}

void RuleMenu::HandleRecruitmentPressed()
{
    SetCurrentTab(RuleMenuTabs::RECRUITMENT);
}

void RuleMenu::Setup()
{
    SetInteractivity(true);

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

    auto realmTab = ElementFactory::BuildElement <RealmTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}, 
            Opacity(0.0f)
        }
    );

    *tabs.Add(RuleMenuTabs::REALM) = realmTab;

    auto buildingTab = ElementFactory::BuildElement <BuildingTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}, 
            Opacity(0.0f)
        }
    );

    *tabs.Add(RuleMenuTabs::BUILDING) = buildingTab;

    auto tradeTab = ElementFactory::BuildElement <TradeTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}, 
            Opacity(0.0f)
        }
    );

    *tabs.Add(RuleMenuTabs::TRADE) = tradeTab;

    auto groupsTab = ElementFactory::BuildElement <GroupTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}
        }
    );

    *tabs.Add(RuleMenuTabs::GROUPS) = groupsTab;

    auto diplomacyTab = ElementFactory::BuildElement <DiplomacyTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}
        }
    );

    *tabs.Add(RuleMenuTabs::DIPLOMACY) = diplomacyTab;

    auto recruitmentTab = ElementFactory::BuildElement <RecruitmentTab>
    (
        {
            size_, 
            drawOrder_, 
            {this}
        }
    );

    *tabs.Add(RuleMenuTabs::RECRUITMENT) = recruitmentTab;

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