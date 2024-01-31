#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"

#include "CombatantModel.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

using namespace battle::render;

static DataBuffer *positionBuffer = nullptr;

static DataBuffer *textureOffsetBuffer = nullptr;

static DataBuffer *opacityBuffer = nullptr;

static DataBuffer *flipBuffer = nullptr;

static const auto MAXIMUM_COMBATANTS_PER_SCENE = 32;

static auto positions = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

static auto offsets = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

static auto opacities = container::Array <float> (MAXIMUM_COMBATANTS_PER_SCENE);

static auto flipStates = container::Array <int> (MAXIMUM_COMBATANTS_PER_SCENE);

static constexpr auto SPRITE_SIZE = 0.4f;

void CombatantModel::Initialize()
{
    positionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

    textureOffsetBuffer = new DataBuffer(offsets.GetMemoryCapacity(), offsets.GetStart());

    opacityBuffer = new DataBuffer(opacities.GetMemoryCapacity(), opacities.GetStart());

    flipBuffer = new DataBuffer(flipStates.GetMemoryCapacity(), flipStates.GetStart());

    camera = RenderManager::GetCamera(Cameras::BATTLE);
}

void CombatantModel::Render()
{
    static auto massShader = ShaderManager::GetShader("ComplexMassSprite");

    static auto unborderedSprite = new Sprite(massShader, "CombatantsComposite");

    static auto borderedSprite = new Sprite(massShader, "CombatantsCompositeBordered");

    positions.Reset();

    offsets.Reset();

    opacities.Reset();

    flipStates.Reset();

    auto playerGroup = BattleScene::Get()->GetPlayerGroup();
    auto computerGroup = BattleScene::Get()->GetComputerGroup();
    for(auto group : {playerGroup, computerGroup})
    {    
        for(auto &combatant : group->GetCombatants())
        {
            auto character = combatant.GetCharacter();

            *positions.Add() = combatant.GetPosition();

            *offsets.Add() = character->GetClass()->TextureData.Offset;

            *opacities.Add() = 1.0f;

            *flipStates.Add() = 0;
        }
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    textureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

    opacityBuffer->UploadData(opacities.GetStart(), opacities.GetMemorySize());

    flipBuffer->UploadData(flipStates.GetStart(), flipStates.GetMemorySize());

    massShader->Bind();

    massShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	massShader->SetConstant(0.0f, "depth");

    massShader->SetConstant(Scale2(0.25f, 0.5f), "textureScale");

    massShader->SetConstant(SPRITE_SIZE, "spriteSize");

    positionBuffer->Bind(0);

    textureOffsetBuffer->Bind(1);

    opacityBuffer->Bind(2);

    flipBuffer->Bind(3);

    unborderedSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());

    if(auto selectedCombatant = BattleController::Get()->GetSelectedCombatant(); selectedCombatant != nullptr)
    {
        positions.Reset();

        offsets.Reset();

        opacities.Reset();

        flipStates.Reset();

        *positions.Add() = selectedCombatant->GetPosition();

        *offsets.Add() = selectedCombatant->GetCharacter()->GetClass()->TextureData.Offset;

        *opacities.Add() = 1.0f;

        *flipStates.Add() = 0;

        positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

        textureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

        opacityBuffer->UploadData(opacities.GetStart(), opacities.GetMemorySize());

        flipBuffer->UploadData(flipStates.GetStart(), flipStates.GetMemorySize());

        borderedSprite->BindDefaultTextures();

        glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());
    }

    massShader->Unbind();
}