#pragma once

#include "FlumenEngine/Render/Model.hpp"

#include "FlumenCore/Singleton.h"

class Shader;

class BattleScene;

class BattleTileModel : public Model, public core::Singleton <BattleTileModel>
{
    friend class Singleton;

    Shader *shader;

    BattleScene *battleScene;

    BattleTileModel();

public:
    void Initialize() override;

    void Render();

    void CreateCamera();

    void HandleCharacterSelected();
};