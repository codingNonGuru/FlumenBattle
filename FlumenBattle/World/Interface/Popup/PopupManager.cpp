#include "FlumenEngine/Interface/ElementFactory.h"

#include "PopupManager.h"
#include "FlumenBattle/World/Interface/Popup/GenericPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Interface/Popup/ExplorationReward.h"

using namespace world::interface::popup;

void PopupManager::Configure()
{
    exploreChestPopups.Initialize(16);

    for(int i = 0; i < exploreChestPopups.GetCapacity(); ++i)
    {
        *exploreChestPopups.Add() = ElementFactory::BuildElement <ExplorationReward>
        (
            {DrawOrder(10), {WorldInterface::Get()->GetCanvas()}}
        );
    }

    exploreChestPopups.Reset();
}

void PopupManager::AddPopup(PopupTypes type)
{
    if(type == PopupTypes::EXPLORATION_REWARD)
    {
        auto popup = *exploreChestPopups.Add();

        popup->Setup();
        popup->Enable();
    }
}

void PopupManager::RemovePopup(PopupTypes type, GenericPopup *popup)
{
    if(type == PopupTypes::EXPLORATION_REWARD)
    {
        exploreChestPopups.Remove(popup);
    }
}