#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"

#include "FlumenBattle/CharacterModel.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/BattleInfoPanel.h"

CharacterInfo* characterInfo = nullptr;

void CharacterModel::Initialize()
{
    auto glyphShader = ShaderManager::GetShaderMap().Get("Glyph");
    TextManager::Initialize(glyphShader);

    battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);

    characterInfos.Initialize(32);

    auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");

    const auto& groups = battleScene->groups;
    for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    {
        const auto& characters = group->characters;
        for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
        {
            auto sprite = new Sprite(nullptr, spriteShader);

            characterInfo = new CharacterInfo();
            Interface::AddElement("CharacterInfo", characterInfo);
            characterInfo->character = character;
            character->info = characterInfo;
            characterInfo->Configure(Size(60, 90), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

            characterInfo->Enable();

            characterInfo->SetInteractivity(true);

            *characterInfos.Allocate() = characterInfo;
        }
    }

    auto sprite = new Sprite(nullptr, spriteShader);

    auto battleInfoPanel = new BattleInfoPanel();
    Interface::AddElement("BattleInfoPanel", battleInfoPanel);
    battleInfoPanel->Configure(Size(620, 140), DrawOrder(1), new Transform(Position2(-640.0f, 460.0f)), sprite, Opacity(1.0f));

    battleInfoPanel->Enable();
}

void CharacterModel::Render(Camera*, Light*)
{
}

