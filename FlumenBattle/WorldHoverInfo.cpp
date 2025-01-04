#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "WorldHoverInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"

using namespace world;

void WorldHoverInfo::HandleConfigure()
{
    static auto color = Color::RED * 0.5f;
    infoLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(5.0f, 5.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"Small"}, color, "Pop 1"}
    );
    infoLabel->SetAlignment(Text::Alignments::LEFT);
    infoLabel->LockWidth(size_.x - 10);
    infoLabel->Enable();

    scienceProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(256, 16), drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, infoLabel}, {"Settings", true}},
        {"SettingsBar", {20.0f, 8.0f}}
    );
    scienceProgress->Disable();
}

void WorldHoverInfo::HandleUpdate()
{
    auto tile = WorldController::Get()->GetHoveredTile();
    if(tile == nullptr)
    {
        DisplayWorldInfo();
        return;
    }

    if(tile->GetSettlement() == nullptr)
    {
        DisplayWorldInfo();
        return;
    }

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

void WorldHoverInfo::DisplayWorldInfo()
{
    auto world = WorldScene::Get();

    Phrase text;
    text << "Settlements: " << world->GetSettlements().GetSize() << "\n";
    text << "Polities: " << world->GetPolities().GetSize() << "\n";
    text << "Encounters: " << world->GetBattles().GetSize() << "\n";
    text << "Groups: " << world->GetGroups().GetSize() << "\n";
    text << "Groups locations: " << [&]
    {
        static auto &batches = group::GroupBatchMap::Get()->GetBatches();

        auto count = 0;
        for(auto batch = batches.GetStart(); batch != batches.GetEnd(); ++batch)
        {
            count += batch->GetGroupCount();
        }

        return count;
    } ();

    infoLabel->Setup(text);

    scienceProgress->Disable();
}