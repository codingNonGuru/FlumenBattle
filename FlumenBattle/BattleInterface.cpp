#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/BattleInterface.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/CombatGroup.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleInfoPanel.h"
#include "FlumenBattle/ActionInfoPanel.h"
#include "FlumenBattle/CharacterDetailPanel.h"
#include "FlumenBattle/BattleEndMessage.h"

#define MAXIMUM_INFO_COUNT 32

#define ELEMENT_OPACITY 0.85f

BattleInterface::BattleInterface()
{
    battleScene = BattleScene::Get();

    characterInfos.Initialize(MAXIMUM_INFO_COUNT);

    canvas = ElementFactory::BuildCanvas();

    auto sprite = SpriteDescriptor("Sprite");

    for(Index i = 0; i < characterInfos.GetCapacity(); ++i)
    {
        auto characterInfo = ElementFactory::BuildElement<CharacterInfo>(
            {Size(60, 90), DrawOrder(1), {Position2(0.0f, 0.0f), canvas}, sprite, Opacity(1.0f)}
        );

        *characterInfos.Allocate() = characterInfo;
    }

    battleInfoPanel = ElementFactory::BuildElement<BattleInfoPanel>(
        {Size(620, 140), DrawOrder(3), {Position2(-640.0f, 460.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    battleInfoPanel->Enable();

    actionInfoPanel = ElementFactory::BuildElement<ActionInfoPanel>(
        {Size(1900, 100), DrawOrder(3), {Position2(0.0f, -480.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    actionInfoPanel->Enable();

    characterDetailPanel = ElementFactory::BuildElement<CharacterDetailPanel>(
        {Size(540, 220), DrawOrder(3), {Position2(680.0f, 420.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
    characterDetailPanel->Enable();

    battleEndMessage = ElementFactory::BuildElement<BattleEndMessage>(
        {Size(300, 180), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, sprite, Opacity(ELEMENT_OPACITY)}
    );
}

void BattleInterface::Initialize()
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

void BattleInterface::Disable()
{
    for(auto iterator = characterInfos.GetStart(); iterator != characterInfos.GetEnd(); ++iterator)
    {
        auto info = *iterator;
        info->Disable();
    }

    canvas->Enable();

    battleEndMessage->Disable();
}

