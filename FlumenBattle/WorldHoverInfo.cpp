#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "WorldHoverInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Science/Technology.h"

using namespace world;

void WorldHoverInfo::HandleConfigure()
{
    static auto color = Color::RED * 0.5f;
    infoLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(5.0f, 5.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"JSLAncient", "Large"}, color, "Pop 1"}
    );
    infoLabel->SetAlignment(Text::Alignments::LEFT);
    infoLabel->LockWidth(size_.x - 10);
    infoLabel->Enable();

    scienceProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(256, 16), drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, infoLabel}, {"Settings", "SlicedSprite"}},
        {"SettingsBar", {20.0f, 8.0f}}
    );
    scienceProgress->Disable();
}

void WorldHoverInfo::HandleUpdate()
{
    auto tile = WorldController::Get()->GetHoveredTile();
    if(tile == nullptr)
        return;

    if(tile->GetSettlement() == nullptr)
        return;

    auto polity = tile->GetSettlement()->GetPolity();
    auto research = polity->GetResearchTarget();

    Phrase text;
    text << polity->GetRuler()->GetName() << "\n";
    text << "Size: " << polity->GetSettlements().GetSize() << "\n";
    text << "Factions: " << polity->GetFactions().GetSize() << "\n";
    text << "Population: " << polity->GetPopulation() << "\n";
    text << "Deaths: " << polity->GetDeathCount() << "\n";
    text << "Science: " << polity->GetScientificPower() << "\n";
    text << "Researching: " << (research != nullptr ? research->Name : "none");

    infoLabel->Setup(text);

    scienceProgress->Enable();
    scienceProgress->SetProgress([&] 
    {
        if(research != nullptr)
            return float(polity->GetResearchProgress()) / float(research->ResearchDuration);
        else
            return 0.0f;
    } ());
}