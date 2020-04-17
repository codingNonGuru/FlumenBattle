#pragma once

#include "FlumenEngine/Render/Model.hpp"

class Shader;

class BattleScene;

class BattleTileModel : public Model
{
    friend class BattleScene;

    Shader *shader;

    BattleScene *battleScene;

    void CreateCamera();

    void HandleCharacterSelected();

    void Initialize() override;

    void Render();
};