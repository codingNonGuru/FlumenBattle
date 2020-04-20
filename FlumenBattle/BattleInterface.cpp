#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"

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

CharacterInfo* characterInfo = nullptr;

BattleInterface::BattleInterface()
{
    std::cout<<"BATTLE INTERFACE START\n";
    auto glyphShader = ShaderManager::GetShaderMap().Get("Glyph");
    TextManager::Initialize(glyphShader);

    battleScene = BattleScene::Get();

    std::cout<<"BATTLE INTERFACE MIDDLE\n";

    characterInfos.Initialize(32);

    auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");


    for(Index i = 0; i < characterInfos.GetCapacity(); ++i)
    {
        auto sprite = new Sprite(nullptr, spriteShader);

        characterInfo = new CharacterInfo();
        Interface::AddElement("CharacterInfo", characterInfo);
        characterInfo->Configure(Size(60, 90), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

        characterInfo->SetInteractivity(true);

        *characterInfos.Allocate() = characterInfo;
    }

    auto sprite = new Sprite(nullptr, spriteShader);

    battleInfoPanel = new BattleInfoPanel();
    Interface::AddElement("BattleInfoPanel", battleInfoPanel);
    battleInfoPanel->Configure(Size(620, 140), DrawOrder(3), new Transform(Position2(-640.0f, 460.0f)), sprite, Opacity(1.0f));

    sprite = new Sprite(nullptr, spriteShader);

    actionInfoPanel = new ActionInfoPanel();
    Interface::AddElement("ActionInfoPanel", actionInfoPanel);
    actionInfoPanel->Configure(Size(1900, 100), DrawOrder(3), new Transform(Position2(0.0f, -480.0f)), sprite, Opacity(1.0f));

    sprite = new Sprite(nullptr, spriteShader);

    characterDetailPanel = new CharacterDetailPanel();
    Interface::AddElement("CharacterDetailPanel", characterDetailPanel);
    characterDetailPanel->Configure(Size(540, 220), DrawOrder(3), new Transform(Position2(680.0f, 420.0f)), sprite, Opacity(1.0f));

    std::cout<<"BATTLE INTERFACE END\n";
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

