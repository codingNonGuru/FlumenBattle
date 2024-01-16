#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Render/Model.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Shader;

namespace world 
{
    class WorldScene;

    class WorldTileModel : public Model, public core::Singleton <WorldTileModel>
    {
        friend class Singleton;

        Shader *shader;

        Shader *groupShader;

        WorldScene *worldScene;

        WorldTileModel();

        void RenderTiles();

        void RenderPaths();

        void RenderBorders();

        void RenderPoliticalOverlay();

        void RenderInterestMap();

        void RenderSettlements();

        void RenderGlobalLight();

        void RenderTilesAdvanced();

        void RenderPlayerPath();

    public:
        void Initialize() override;

        void Render();

        void CreateCamera();

        void HandleCharacterSelected();

        Color GetGlobalLightColor();
    };
}