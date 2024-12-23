#include "FlumenEngine/Interface/ElementFactory.h"

#include "PopupManager.h"
#include "FlumenBattle/World/Interface/Popup/GenericPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Interface/Popup/ExplorationReward.h"
#include "FlumenBattle/World/Interface/Popup/BattleStartPopup.h"

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

    
    battleStartPopups.Initialize(32);

    for(int i = 0; i < battleStartPopups.GetCapacity(); ++i)
    {
        *battleStartPopups.Add() = ElementFactory::BuildElement <BattleStartPopup>
        (
            {DrawOrder(10), {WorldInterface::Get()->GetCanvas()}}
        );
    }

    battleStartPopups.Reset();
}

void PopupManager::AddPopup(PopupTypes type)
{
    auto popup = [&] -> GenericPopup *
    {
        if(type == PopupTypes::EXPLORATION_REWARD)
        {
            return *exploreChestPopups.Add();    
        }
        else if(type == PopupTypes::BATTLE_START)
        {
            return *battleStartPopups.Add();    
        }
    } ();

    popup->Setup();
    popup->Enable();
}

void PopupManager::RemovePopup(PopupTypes type, GenericPopup *popup)
{
    if(type == PopupTypes::EXPLORATION_REWARD)
    {
        exploreChestPopups.Remove(popup);
    }
    else if(type == PopupTypes::BATTLE_START)
    {
        battleStartPopups.Remove(popup);
    }
}