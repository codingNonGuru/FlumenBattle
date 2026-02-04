#include "FlumenEngine/Interface/ElementFactory.h"

#include "JobItem.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void JobItem::Setup(polity::WorkInstruction *instruction, bool isWorkerHired) 
{
    this->instruction = instruction;

    if(instruction != nullptr && instruction->PlaceType == polity::WorkInstruction::RESOURCE)
    {
        resourceIcon->Enable();
        resourceIcon->SetTexture(instruction->Place.Resource->Type->TextureName);

        resourceIcon->SetOpacity(isWorkerHired == true ? 1.0f : 0.5f);

        priority = instruction->Priority;

        SetInteractivity(true);

        priorityLabel->Enable();
    }
    else
    {
        resourceIcon->Disable();

        SetInteractivity(false);

        priorityLabel->Disable();
    }
}

void JobItem::HandleConfigure() 
{
    resourceIcon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, 
            {"Radish", false}
        }
    );
    resourceIcon->Disable();

    priorityLabel = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, this}, {"WhiteDotBackdrop", false}}
    );
    priorityLabel->Setup(&priority, Scale2(0.6f), "Small");
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
    polity::HumanMind::Get()->RemoveWorkInstruction(instruction);
}