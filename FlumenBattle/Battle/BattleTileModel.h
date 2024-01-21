#pragma once

#include "FlumenEngine/Render/Model.hpp"

#include "FlumenCore/Singleton.h"

class Shader;

namespace battle
{
    class BattleScene;

    class BattleTileModel : public Model, public core::Singleton <BattleTileModel>
    {
        friend class core::Singleton <BattleTileModel>;

        Shader *shader;

        BattleScene *battleScene;

        BattleTileModel();

        void RenderActionRange();

    public:
        void Initialize() override;

        void Render();

        void CreateCamera();

        void HandleCharacterSelected();
    };
}