#include <chrono>

#include "FlumenCore/Observer.h"
#include "FlumenCore/Container/Queue.h"

#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/LineRenderer.h"

#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleController.h"
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
#include "FlumenBattle/Battle/Interface/ConditionPopup.h"
#include "FlumenBattle/Battle/Interface/TargetingCard.h"

using namespace battle;

#define MAXIMUM_INFO_COUNT 32

#define ELEMENT_OPACITY 0.85f

const auto MAXIMUM_DAMAGE_COUNTERS = 32;

static const auto CONDITION_POPUP_CAPACITY = 32;

#define TIME_BETWEEN_FADING_POPUPS 300

static Combatant *hoveredCombatant = nullptr;

struct PopupData
{
    world::character::Conditions ConditionType;
};

struct CombatantPopupData
{
    Combatant *Combatant;

    std::chrono::_V2::steady_clock::time_point LastPopupTimestamp;

    container::Queue <PopupData> PopupQueue = container::Queue <PopupData> (8);

    CombatantPopupData() {}

    CombatantPopupData(class Combatant *combatant) : Combatant(combatant), LastPopupTimestamp(std::chrono::steady_clock::now()) {}
};

container::Array <CombatantPopupData> combatantPopupDatas;

BattleInterface::BattleInterface()
{
    battleScene = BattleScene::Get();

    characterInfos.Initialize(MAXIMUM_INFO_COUNT);

    canvas = ElementFactory::BuildCanvas();
    canvas->SetIdentifier("BattleCanvas");

    auto sprite = SpriteDescriptor(false);

    for(Index i = 0; i < characterInfos.GetCapacity(); ++i)
    {
        auto characterInfo = ElementFactory::BuildElement<CharacterInfo>(
            {Size(60, 90), DrawOrder(3), {Position2(), canvas}, sprite, Opacity(0.5f)}
        );

        characterInfo->SetIdentifier(Word() << "CharacterInfo" << i);

        *characterInfos.Allocate() = characterInfo;
        characterInfo->SetInteractivity(true);
    }

    combatantPopupDatas.Initialize(MAXIMUM_INFO_COUNT);

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
            {DrawOrder(10), {canvas}}
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

    conditionPopups.Initialize(CONDITION_POPUP_CAPACITY);
    for(int i = 0; i < CONDITION_POPUP_CAPACITY; ++i)
    {
        *conditionPopups.Add() = ElementFactory::BuildElement <interface::ConditionPopup>
        (
            {DrawOrder(6), {canvas}}
        );
    }

    conditionPopups.Reset();

    targetingCard = ElementFactory::BuildElement <interface::TargetingCard>
    (
        {
            Size(90, 50), 
            DrawOrder(4), 
            {canvas}, 
            {false}, 
            Opacity(0.6f)
        }
    );
    targetingCard->Disable();
}

void BattleInterface::Initialize()
{
    *HumanController::Get()->OnTargetInitiated += {this, &BattleInterface::HandleTargetInitiated};

    *HumanController::Get()->OnTargetAbandoned += {this, &BattleInterface::HandleTargetAbandoned};

    BattleScene::Get()->OnConditionAdded += {this, &BattleInterface::HandleConditionApplied};
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

            *combatantPopupDatas.Allocate() = {combatant};
        }
    }

    Engine::OnInterfaceUpdateStarted += {this, &BattleInterface::Update};
}

void BattleInterface::Update()
{
    for(auto &popupData : combatantPopupDatas)
    {
        if(popupData.PopupQueue.IsEmpty() == true)
            continue;

        auto timestamp = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupData.LastPopupTimestamp).count() > TIME_BETWEEN_FADING_POPUPS)
        {
            auto data = popupData.PopupQueue.Pop();

            auto popup = *conditionPopups.Add();

            popup->Setup(popupData.Combatant, data->ConditionType);

            popup->Enable();

            popupData.LastPopupTimestamp = timestamp;
        }
    }

    static auto battleController = BattleController::Get();

    static auto humanController = HumanController::Get();

    bool isHoveringTargetWhileInitiating = humanController->IsInitiatingTargeting() == true && humanController->GetHoveredTile() != nullptr && humanController->GetHoveredTile()->Combatant != nullptr;
    if(isHoveringTargetWhileInitiating == true)
    {
        targetingCard->Setup(battleController->GetSelectedCombatant(), humanController->GetHoveredTile()->Combatant);
        targetingCard->Enable();
    }
    else
    {
        targetingCard->Disable();
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

    combatantPopupDatas.Reset();

    canvas->Disable();

    battleEndMessage->Disable();

    Engine::OnInterfaceUpdateStarted -= {this, &BattleInterface::Update};
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

void BattleInterface::RemoveConditionPopup(interface::ConditionPopup *popup)
{
    conditionPopups.Remove(popup);
}

void BattleInterface::HandleConditionApplied()
{
    auto timestamp = std::chrono::steady_clock::now();

    auto conditionData = BattleScene::Get()->GetLatestConditionData();

    for(auto &popupData : combatantPopupDatas)
    {
        if(conditionData.Combatant != popupData.Combatant)
            continue;

        if(std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - popupData.LastPopupTimestamp).count() < TIME_BETWEEN_FADING_POPUPS)
        {
            *popupData.PopupQueue.Grow() = {conditionData.Type};
        }
        else
        {
            auto popup = *conditionPopups.Add();

            popup->Setup(conditionData.Combatant, conditionData.Type);

            popup->Enable();

            popupData.LastPopupTimestamp = timestamp;
        }

        break;
    }
}

