#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "PopulationItem.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void PopulationItem::HandleConfigure()
{
    /*raceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "X"}
    );
    raceLabel->Disable();*/

    raceIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
            {"Radish", false}
        }
    );
    //raceIcon->SetTextureScale(0.7f);
    raceIcon->Enable();

    healthBar = ElementFactory::BuildProgressBar <ProgressBar>
    (
        {Size(52, 20), drawOrder_, {Position2(0.0f, -4.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    healthBar->Enable();

    experienceBar = ElementFactory::BuildProgressBar <ProgressBar>
    (
        {Size(52, 16), drawOrder_, {Position2(0.0f, -24.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerCyan", {6.0f, 6.0f}}
    );
    experienceBar->Enable();

    jobSprite = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 2, {Position2(15.0f, 10.0f), this}, {"Plank", false}}
    );
    jobSprite->SetTextureScale(0.9f);
    jobSprite->Enable();

    levelCounter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(-5.0f, 5.0f), ElementAnchors::UPPER_RIGHT, ElementPivots::UPPER_RIGHT, this}, {"WhiteDotBackdrop", false}}
    );
    levelCounter->Setup(&level, Scale2(0.85f), "Medium");
    levelCounter->Enable();

    SetInteractivity(true);
}

void PopulationItem::HandleUpdate()
{
    auto ratio = (float)cohort->Health / (float)cohort->MAXIMUM_HEALTH;

    healthBar->SetProgress(ratio);

    experienceBar->SetProgress(cohort->GetExperienceRatio());

    if(cohort->Job == nullptr)
        jobSprite->Disable();
    else
    {
        
        auto resource = [&] 
        {
            if(cohort->Job->GetTile() == nullptr)
                return settlement::ResourceFactory::Get()->CreateType(cohort->Job->GetResource());
            else
            {
                auto type = cohort->Job->GetTile()->Tile->GetMajorResource();
                return settlement::ResourceFactory::Get()->CreateType(type);
            }
        } ();
        
        jobSprite->SetTexture(resource->TextureName);
        jobSprite->Enable();
    }

    level = cohort->GetLevel();
}

void PopulationItem::HandleLeftClick()
{
    if(cohort->Job == nullptr)
        return;

    polity::HumanMind::Get()->RemoveWorkInstruction(this->cohort);
}

void PopulationItem::Setup(const settlement::Cohort *cohort, const settlement::Settlement *settlement)
{
    this->settlement = settlement;

    this->cohort = cohort;

    //raceLabel->Setup(Word() << cohort->Race->Name[0]);

    raceIcon->SetTexture(cohort->Race->TextureName);
}