#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "ProductionQueueItem.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void ProductionQueueItem::HandleConfigure() 
{
    label = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 5.0f), this}}, 
        {{"Medium"}, TEXT_COLOR}
    );
    label->Enable();

    SetInteractivity(true);
}

void ProductionQueueItem::HandleUpdate()
{
    if(this->IsHovered() == true)
    {
        SetOpacity(1.0f);

        if(queueSlot->Tile != nullptr)
        {
            polity::HumanMind::Get()->SetHoveredBuildingQueueItem({queueSlot->Tile, queueSlot->ImprovementIndex});
        }
    }
    else
    {
        SetOpacity(0.5f);

        if(queueSlot->Tile == polity::HumanMind::Get()->GetHoveredBuildingQueueItem().Tile)
        {
            polity::HumanMind::Get()->SetHoveredBuildingQueueItem({nullptr});
        }
    }
}

void ProductionQueueItem::HandleLeftClick() 
{
    polity::HumanMind::Get()->CancelProduction(queueSlot);

    if(queueSlot->Tile == polity::HumanMind::Get()->GetHoveredBuildingQueueItem().Tile)
    {
        polity::HumanMind::Get()->SetHoveredBuildingQueueItem({nullptr});
    }
}

void ProductionQueueItem::Setup(polity::ProductionQueueSlot *queueSlot)
{
    this->queueSlot = queueSlot;

    auto type = settlement::SettlementProduction::GetType(queueSlot->Option);

    label->Setup(type->Name.GetFirstCharacter());
}