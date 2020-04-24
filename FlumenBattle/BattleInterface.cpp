#include "FlumenEngine/Core/SceneManager.hpp"

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

CharacterInfo* characterInfo = nullptr;

BattleInterface::BattleInterface()
{
    battleScene = BattleScene::Get();

    characterInfos.Initialize(MAXIMUM_INFO_COUNT);

    auto sprite = SpriteDescriptor("Sprite");

    for(Index i = 0; i < characterInfos.GetCapacity(); ++i)
    {
        characterInfo = new CharacterInfo();
        characterInfo->Configure(Size(60, 90), DrawOrder(1), Position2(0.0f, 0.0f), sprite, Opacity(1.0f));

        characterInfo->SetInteractivity(true);

        *characterInfos.Allocate() = characterInfo;
    }

    battleInfoPanel = new BattleInfoPanel();
    battleInfoPanel->Configure(Size(620, 140), DrawOrder(3), Position2(-640.0f, 460.0f), sprite, Opacity(ELEMENT_OPACITY));

    actionInfoPanel = new ActionInfoPanel();
    actionInfoPanel->Configure(Size(1900, 100), DrawOrder(3), Position2(0.0f, -480.0f), sprite, Opacity(ELEMENT_OPACITY));

    characterDetailPanel = new CharacterDetailPanel();
    characterDetailPanel->Configure(Size(540, 220), DrawOrder(3), Position2(680.0f, 420.0f), sprite, Opacity(ELEMENT_OPACITY));

    battleEndMessage = new BattleEndMessage();
    battleEndMessage->Configure(Size(300, 180), DrawOrder(3), Position2(0.0f, 0.0f), sprite, Opacity(ELEMENT_OPACITY));
}

void BattleInterface::Initialize()
{
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

    battleInfoPanel->Enable();

    actionInfoPanel->Enable();

    characterDetailPanel->Enable();
}

void BattleInterface::Disable()
{
    for(auto iterator = characterInfos.GetStart(); iterator != characterInfos.GetEnd(); ++iterator)
    {
        auto info = *iterator;
        info->Disable();
    }

    battleInfoPanel->Disable();

    actionInfoPanel->Disable();

    characterDetailPanel->Disable();

    battleEndMessage->Disable();
}

