#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "NeighborItem.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/Neighbor.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void NeighborItem::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    populationCounter = ElementFactory::BuildElement <interface::ResourceCounter> (
        {drawOrder_ + 1, {Position2(180.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}
    );
    populationCounter->Setup("WillSave", &population);
    populationCounter->SetOffset(3.0f);
    populationCounter->Enable();

    settlementCounter = ElementFactory::BuildElement <interface::ResourceCounter> (
        {drawOrder_ + 1, {Position2(260.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}
    );
    settlementCounter->Setup("Houses", &settlementCount);
    settlementCounter->SetIconScale(0.3f);
    settlementCounter->SetOffset(-40.0f);
    settlementCounter->Enable();

    linkCounter = ElementFactory::BuildElement <interface::ResourceCounter> (
        {drawOrder_ + 1, {Position2(340.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}
    );
    linkCounter->Setup("SmallMap", &linkCount);
    linkCounter->SetOffset(3.0f);
    linkCounter->Enable();
}

void NeighborItem::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(HOVERED_OPTION_OPACITY);
    }
    else
    {
        SetOpacity(BASE_OPTION_OPACITY);
    }

    population = neighbor->Polity->GetPopulation();

    settlementCount = neighbor->Polity->GetSettlements().GetSize();

    linkCount = neighbor->LinkCount;
}

void NeighborItem::Setup(const polity::Neighbor *newNeighbor, DiplomacyTab *)
{
    neighbor = newNeighbor;

    nameLabel->Setup(neighbor->Polity->GetRuler()->GetName());
}