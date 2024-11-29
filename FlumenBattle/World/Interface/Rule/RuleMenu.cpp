#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "RuleMenu.h"
#include "EconomyTab.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Race.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

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

    economyTab->Enable();
}

void RuleMenu::SetCurrentSettlement(settlement::Settlement *settlement) 
{
    this->settlement = settlement;

    OnSettlementChanged.Invoke();
}