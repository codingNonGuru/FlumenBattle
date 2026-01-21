#include "FlumenEngine/Interface/ElementFactory.h"

#include "PopulationItem.h"
#include "FlumenBattle/World/Settlement/Cohort.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void PopulationItem::HandleConfigure()
{
    raceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "X"}
    );
    raceLabel->Enable();
}

void PopulationItem::HandleUpdate()
{

}

void PopulationItem::Setup(const settlement::Cohort *cohort)
{
    this->cohort = cohort;

    raceLabel->Setup(Word() << cohort->Race->Name[0]);
}