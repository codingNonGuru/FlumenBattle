#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "DomainItem.h"
#include "RealmTab.h"
#include "DomainHoverInfo.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Building.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

void DomainItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    nameLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {Position2(10.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Medium"}, TEXT_COLOR, HIGHLIGHT_COLOR}
    );
    nameLabel->Enable();

    capitalIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_RIGHT, this}, 
            {"CrownCoin", false}
        }
    );
    capitalIcon->AdjustSizeToTexture();

    populationCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(200.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    populationCounter->Setup("PopulationIcon", &population, "Medium");
    populationCounter->SetOffset(5.0f);
    populationCounter->Enable();

    housingCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(280.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    housingCounter->Setup("House2_32", &housing, "Medium", Scale2(0.9f));
    housingCounter->SetOffset(5.0f);
    housingCounter->Enable();

    distanceCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(360.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    distanceCounter->Setup("Boot2_32", &distance, "Medium");
    distanceCounter->SetOffset(5.0f);
    distanceCounter->Enable();

    scienceCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(440.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    scienceCounter->Setup("ScienceIcon", &science, "Medium");
    scienceCounter->SetOffset(5.0f);
    scienceCounter->Enable();

    industryCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(520.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    industryCounter->Setup("WorkHammer", &industry, "Medium");
    industryCounter->SetOffset(5.0f);
    industryCounter->Enable();

    SetInteractivity(true);
}

void DomainItem::HandleUpdate()
{
    if(isHovered_ == true)
    {
        SetOpacity(1.0f);

        auto hoverInfo = parentTab->GetHoverInfo();
        hoverInfo->Setup(this);
        hoverInfo->Enable();
    }
    else
    {
        SetOpacity(0.5f);
    }
}

void DomainItem::Setup(settlement::Settlement *settlement, RealmTab *tab)
{
    this->settlement = settlement;

    this->parentTab = tab;

    auto text = Word("<2>") << settlement->GetName().GetFirstCharacter() << "<1>" << (settlement->GetName().Get() + 1);
    nameLabel->Setup(text);

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();
    if(playerPolity->GetRuler() == settlement)
    {
        capitalIcon->Enable();
    }
    else
    {
        capitalIcon->Disable();
    }

    population = settlement->GetPopulation();

    housing = settlement->GetBuilding(settlement::BuildingTypes::HOUSING).GetAmount();

    distance = settlement->GetDistanceToCapital();

    industry = settlement->GetIndustrialProduction();

    science = settlement->GetScienceProduction();
}