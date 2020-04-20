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

void BattleInterface::Initialize()
{
    auto glyphShader = ShaderManager::GetShaderMap().Get("Glyph");
    TextManager::Initialize(glyphShader);

    battleScene = BattleScene::Get();

    characterInfos.Initialize(32);

    auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");

    auto groups = {battleScene->GetPlayerGroup(), battleScene->GetComputerGroup()};
    //for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    for(auto group : groups)
    {
        const auto& combatants = group->GetCombatants();
        for(auto combatant = combatants.GetStart(); combatant != combatants.GetEnd(); ++combatant)
        {
            auto sprite = new Sprite(nullptr, spriteShader);

            characterInfo = new CharacterInfo();
            Interface::AddElement("CharacterInfo", characterInfo);
            characterInfo->SetCombatant(combatant);
            characterInfo->Configure(Size(60, 90), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

            characterInfo->Enable();

            characterInfo->SetInteractivity(true);

            *characterInfos.Allocate() = characterInfo;
        }
    }

    auto sprite = new Sprite(nullptr, spriteShader);

    auto battleInfoPanel = new BattleInfoPanel();
    Interface::AddElement("BattleInfoPanel", battleInfoPanel);
    battleInfoPanel->Configure(Size(620, 140), DrawOrder(3), new Transform(Position2(-640.0f, 460.0f)), sprite, Opacity(1.0f));

    battleInfoPanel->Enable();

    sprite = new Sprite(nullptr, spriteShader);

    auto actionInfoPanel = new ActionInfoPanel();
    Interface::AddElement("ActionInfoPanel", actionInfoPanel);
    actionInfoPanel->Configure(Size(1900, 100), DrawOrder(3), new Transform(Position2(0.0f, -480.0f)), sprite, Opacity(1.0f));

    actionInfoPanel->Enable();

    sprite = new Sprite(nullptr, spriteShader);

    auto characterDetailPanel = new CharacterDetailPanel();
    Interface::AddElement("CharacterDetailPanel", characterDetailPanel);
    characterDetailPanel->Configure(Size(540, 220), DrawOrder(3), new Transform(Position2(680.0f, 420.0f)), sprite, Opacity(1.0f));

    characterDetailPanel->Enable();
}

