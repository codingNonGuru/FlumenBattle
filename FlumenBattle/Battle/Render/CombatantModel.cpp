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

static DataBuffer *rotationBuffer = nullptr;

static const auto MAXIMUM_COMBATANTS_PER_SCENE = 32;

static auto positions = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

static auto offsets = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

static auto rotations = container::Array <float> (MAXIMUM_COMBATANTS_PER_SCENE);

static constexpr auto SPRITE_SIZE = 0.4f;

void CombatantModel::Initialize()
{
    positionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

    textureOffsetBuffer = new DataBuffer(offsets.GetMemoryCapacity(), offsets.GetStart());

    rotationBuffer = new DataBuffer(rotations.GetMemoryCapacity(), rotations.GetStart());

    camera = RenderManager::GetCamera(Cameras::BATTLE);
}

void CombatantModel::Render()
{
    static auto massShader = ShaderManager::GetShader("ComplexMassSprite");

    static auto unborderedSprite = new Sprite(massShader, "CombatantsComposite");

    static auto borderedSprite = new Sprite(massShader, "CombatantsCompositeBordered");

    positions.Reset();

    offsets.Reset();

    rotations.Reset();

    auto playerGroup = BattleScene::Get()->GetPlayerGroup();
    auto computerGroup = BattleScene::Get()->GetComputerGroup();
    for(auto group : {playerGroup, computerGroup})
    {   
        for(auto &combatant : group->GetCombatants())
        {
            if(&combatant == BattleController::Get()->GetSelectedCombatant())
                continue;

            auto character = combatant.GetCharacter();

            *positions.Add() = combatant.GetPosition();

            *offsets.Add() = character->GetClass()->TextureData.Offset;

            *rotations.Add() = combatant.GetRotation() + PI / 3.0f;
        }
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    textureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

    rotationBuffer->UploadData(rotations.GetStart(), rotations.GetMemorySize());

    massShader->Bind();

    massShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	massShader->SetConstant(0.0f, "depth");

    massShader->SetConstant(Scale2(0.25f, 0.5f), "textureScale");

    massShader->SetConstant(SPRITE_SIZE, "spriteSize");

    massShader->SetConstant(1, "hasRotation");

    massShader->SetConstant(0, "hasOpacity");

    massShader->SetConstant(0, "hasFlip");

    positionBuffer->Bind(0);

    textureOffsetBuffer->Bind(1);

    rotationBuffer->Bind(4);

    unborderedSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());

    if(auto selectedCombatant = BattleController::Get()->GetSelectedCombatant(); selectedCombatant != nullptr)
    {
        positions.Reset();

        offsets.Reset();

        rotations.Reset();

        *positions.Add() = selectedCombatant->GetPosition();

        *offsets.Add() = selectedCombatant->GetCharacter()->GetClass()->TextureData.Offset;

        *rotations.Add() = selectedCombatant->GetRotation() + PI / 3.0f;

        positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

        textureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

        rotationBuffer->UploadData(rotations.GetStart(), rotations.GetMemorySize());

        borderedSprite->BindDefaultTextures();

        glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());
    }

    massShader->Unbind();
}