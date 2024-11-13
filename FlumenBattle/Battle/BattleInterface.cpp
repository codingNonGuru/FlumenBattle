#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleInfoPanel.h"
#include "FlumenBattle/Battle/Interface/ActionInfoPanel.h"
#include "FlumenBattle/Battle/CharacterDetailPanel.h"
#include "FlumenBattle/Battle/BattleEndMessage.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/Battle/Interface/BattleCounter.h"
#include "FlumenBattle/Battle/Interface/CharacterHoverInfo.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/Interface/TargetCursor.h"

#define MAXIMUM_INFO_COUNT 32

#define ELEMENT_OPACITY 0.85f

const auto MAXIMUM_DAMAGE_COUNTERS = 32;

using namespace battle;

static Combatant *hoveredCombatant = nullptr;

BattleInterface::BattleInterface()
{
    battleScene = BattleScene::Get();

    characterInfos.Initialize(MAXIMUM_INFO_COUNT);

    canvas = ElementFactory::BuildCanvas();

    auto sprite = SpriteDescriptor(false);

    for(Index i = 0; i < characterInfos.GetCapacity(); ++i)
    {
        auto characterInfo = ElementFactory::BuildElement<CharacterInfo>(
            {Size(60, 90), DrawOrder(1), {Position2(), canvas}, sprite, Opacity(0.5f)}
        );

        *characterInfos.Allocate() = characterInfo;
        characterInfo->SetInteractivity(true);
    }

    battleInfoPanel = ElementFactory::BuildElement<BattleInfoPanel>(
        {Size(620, 140), DrawOrder(3), {Position2(-640.0f, 460.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    battleInfoPanel->Enable();

    actionInfoPanel = ElementFactory::BuildElement <interface::ActionInfoPanel>(
        {Size(1900, 100), DrawOrder(3), {Position2(0.0f, -480.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    actionInfoPanel->Enable();

    characterDetailPanel = ElementFactory::BuildElement<CharacterDetailPanel>(
        {Size(540, 220), DrawOrder(3), {Position2(680.0f, 420.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    characterDetailPanel->Enable();

    battleEndMessage = ElementFactory::BuildElement<BattleEndMessage>(
        {Size(720, 180), DrawOrder(3), {canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );

    damageCounters.Initialize(MAXIMUM_DAMAGE_COUNTERS);
    for(Index i = 0; i < damageCounters.GetCapacity(); ++i)
    {
        auto counter = ElementFactory::BuildElement <interface::BattleCounter>
        (
            {DrawOrder(1), {canvas}}
        );

        *damageCounters.Add() = counter;
    }

    damageCounters.Reset();

    hoverInfo = ElementFactory::BuildElement <interface::CharacterHoverInfo>
    (
        {
            Size(200, 200), 
            DrawOrder(4), 
            {canvas}, 
            {false}, 
            Opacity(0.6f)
        }
    );
    hoverInfo->Disable();

    targetCursor = ElementFactory::BuildElement <interface::TargetCursor>
    (
        {
            DrawOrder(3), 
            {canvas}, 
            {false}
        }
    );
    targetCursor->FollowMouse();
    targetCursor->Disable();
}

void BattleInterface::Initialize()
{
    *HumanController::Get()->OnTargetInitiated += {this, &BattleInterface::HandleTargetInitiated};

    *HumanController::Get()->OnTargetAbandoned += {this, &BattleInterface::HandleTargetAbandoned};
}

void BattleInterface::Enable()
{
    canvas->Enable();

    auto characterInfo = characterInfos.GetStart();
    auto groups = {battleScene->GetPlayerGroup(), battleScene->GetComputerGroup()};
    for(auto group : groups)
    {
        const auto& combatants = group->GetCombatants();
        for(auto combatant = combatants.GetStart(); combatant != combatants.GetEnd(); ++combatant, ++characterInfo)
        {
            (*characterInfo)->SetCombatant(combatant);
            (*characterInfo)->Enable();
        }
    }
}

void BattleInterface::HandleTargetInitiated()
{
    targetCursor->Enable();
}

void BattleInterface::HandleTargetAbandoned()
{
    targetCursor->Disable();
}

void BattleInterface::Disable()
{
    for(auto iterator = characterInfos.GetStart(); iterator != characterInfos.GetEnd(); ++iterator)
    {
        auto info = *iterator;
        info->Disable();
    }

    canvas->Disable();

    battleEndMessage->Disable();
}

Combatant *BattleInterface::GetHoveredCombatant()
{
    return hoveredCombatant;
}

interface::BattleCounter *BattleInterface::GetDamageCounter(int value, Combatant *combatant)
{
    auto counter = *damageCounters.Add();
    counter->Setup(value, combatant);
    counter->Enable();

    return counter;
}

void BattleInterface::RemoveDamageCounter(interface::BattleCounter *counter)
{
    damageCounters.Remove(counter);
}

void BattleInterface::EnableHoverExtension(CharacterInfo *characterInfo)
{
    hoveredCombatant = characterInfo->GetCombatant();

    hoverInfo->Setup(characterInfo);
    hoverInfo->Enable();
}

