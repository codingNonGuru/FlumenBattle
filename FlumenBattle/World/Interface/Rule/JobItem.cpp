#include "FlumenEngine/Interface/ElementFactory.h"

#include "JobItem.h"
#include "EconomyTab.h"
#include "JobHoverInfo.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Tile/WorldTile.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void JobItem::Setup(polity::WorkInstruction *instruction, EconomyTab *parentTab, bool isWorkerHired) 
{
    this->instruction = instruction;
    
    this->parentTab = parentTab;

    if(instruction != nullptr)
    {
        resourceIcon->Enable();
        cohortIcon->Enable();

        if(instruction->PlaceType == polity::WorkInstruction::RESOURCE)
        {
            resourceIcon->SetTexture(instruction->Place.Resource->Type->TextureName);
        }
        else
        {
            auto majorResource = instruction->Place.Tile->Tile->GetMajorResource();
            auto resourceType = settlement::ResourceFactory::Get()->CreateType(majorResource);
            resourceIcon->SetTexture(resourceType->TextureName);
        }

        cohortIcon->SetTexture(instruction->Cohort->Race->TextureName);

        resourceIcon->SetOpacity(isWorkerHired == true ? 1.0f : 0.5f);

        priority = instruction->Priority + 1;

        SetInteractivity(true);

        priorityLabel->Enable();
    }
    else
    {
        resourceIcon->Disable();

        cohortIcon->Disable();

        SetInteractivity(false);

        priorityLabel->Disable();
    }
}

void JobItem::HandleConfigure() 
{
    resourceIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 2, 
            {Position2(8.0f, 12.0f), this}, 
            {"Radish", false}
        }
    );
    resourceIcon->SetTextureScale(0.7f);
    resourceIcon->Disable();

    cohortIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(-2.0f, -4.0f), this}, 
            {"Radish", false}
        }
    );
    cohortIcon->SetTextureScale(0.7f);
    cohortIcon->Disable();

    priorityLabel = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, this}, {"WhiteDotBackdrop", false}}
    );
    priorityLabel->Setup(&priority, Scale2(0.7f), "Medium");
    priorityLabel->Disable();
}

void JobItem::HandleUpdate()
{
    if(instruction == nullptr)
    {
        SetOpacity(0.2f);
        return;
    }

    if(IsHovered() == true)
    {
        SetOpacity(0.8f);
    }
    else
    {
        SetOpacity(0.5f);
    }
}

void JobItem::HandleLeftClick() 
{
    if(instruction == nullptr)
        return;

    auto hoverInfo = parentTab->GetJobHoverDevice();

    hoverInfo->Disable();

    polity::HumanMind::Get()->RemoveWorkInstruction(instruction);

    instruction = nullptr;
}

void JobItem::HandleHover()
{
    if(this->instruction == nullptr)
        return;

    auto hoverInfo = parentTab->GetJobHoverDevice();

    hoverInfo->Setup(this, instruction);

    hoverInfo->Enable();
}