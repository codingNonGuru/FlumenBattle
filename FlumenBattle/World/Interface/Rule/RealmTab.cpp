#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RealmTab.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Settlement/Building.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void DomainItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Medium"}, TEXT_COLOR}
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

    housingCounter->Setup("Houses64", &housing, "Medium", Scale2(0.5f));
    housingCounter->SetOffset(-5.0f);
    housingCounter->Enable();

    distanceCounter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(360.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, this}}
    );

    distanceCounter->Setup("LeatherBoot", &distance, "Medium");
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
    }
    else
    {
        SetOpacity(0.5f);
    }
}

void DomainItem::Setup(settlement::Settlement *settlement)
{
    this->settlement = settlement;

    nameLabel->Setup(settlement->GetName());

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

void RealmTab::HandleConfigure()
{
    static const auto MAXIMUM_POLITY_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_POLITY_SIZE).Integer;

    domainLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 80.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        MAXIMUM_POLITY_SIZE
    );
    domainLayout->SetDistancing(1, 5.0f);
    domainLayout->Enable();

    domainItems.Initialize(MAXIMUM_POLITY_SIZE);
    for(auto i = 0; i < domainItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <DomainItem>
        (
            {
                Size(580, 35), 
                drawOrder_ + 1, 
                {domainLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *domainItems.Allocate() = item;
    }
}

void RealmTab::HandleUpdate()
{
    for(auto &item : domainItems)
        item->Disable();

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto item = domainItems.GetStart();
    for(auto &settlement : playerPolity->GetSettlements())
    {
        (*item)->Setup(settlement);
        (*item)->Enable();

        item++;
    }
}