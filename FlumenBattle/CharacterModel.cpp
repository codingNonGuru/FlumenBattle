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

CharacterInfo* characterInfo = nullptr;

void CharacterModel::Initialize()
{
    auto glyphShader = ShaderManager::GetShaderMap().Get("Glyph");
    TextManager::Initialize(glyphShader);

    battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);

    characterInfos.Initialize(32);

    const auto& groups = battleScene->groups;
    for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    {
        const auto& characters = group->characters;
        for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
        {
            auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");
            auto sprite = new Sprite(nullptr, spriteShader);

            characterInfo = new CharacterInfo();
            Interface::AddElement("Character", characterInfo);
            characterInfo->character = character;
            character->info = characterInfo;
            characterInfo->Configure(Size(90, 130), DrawOrder(1), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(1.0f));

            characterInfo->Enable();

            characterInfo->SetInteractivity(true);

            *characterInfos.Allocate() = characterInfo;
        }
    }
}

void CharacterModel::Render(Camera*, Light*)
{
    //battleScene->Render()
}

