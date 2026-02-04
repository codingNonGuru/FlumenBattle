#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "PopulationItem.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void PopulationItem::HandleConfigure()
{
    raceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "X"}
    );
    raceLabel->Enable();

    healthBar = ElementFactory::BuildProgressBar <ProgressBar>
    (
        {Size(52, 24), drawOrder_ + 1, {Position2(0.0f, -4.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    healthBar->Enable();

    jobSprite = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 1, {Position2(10.0f, 0.0f), this}, {"Plank", false}}
    );
    jobSprite->SetTextureScale(0.8f);
    jobSprite->Enable();

    SetInteractivity(true);
}

void PopulationItem::HandleUpdate()
{
    auto ratio = (float)cohort->Health / (float)cohort->MAXIMUM_HEALTH;

    healthBar->SetProgress(ratio);

    if(cohort->Job == nullptr)
        jobSprite->Disable();
    else
    {
        auto resource = settlement::ResourceFactory::Get()->CreateType(cohort->Job->GetResource());
        
        jobSprite->SetTexture(resource->TextureName);
        jobSprite->Enable();
    }
}

void PopulationItem::HandleLeftClick()
{
    if(cohort->Job == nullptr)
        return;

    settlement->FireWorker(cohort->Job);
}

void PopulationItem::Setup(const settlement::Cohort *cohort, const settlement::Settlement *settlement)
{
    this->settlement = settlement;

    this->cohort = cohort;

    raceLabel->Setup(Word() << cohort->Race->Name[0]);
}