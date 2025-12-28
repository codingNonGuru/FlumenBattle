#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Render/Model.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Shader;
namespace render
{
    class Texture;
}

namespace world
{
    class WorldScene;
    class WorldGenerator;
}

namespace world::render 
{
    class WorldTileModel : public Model, public core::Singleton <WorldTileModel>
    {
        friend class Singleton;

        friend class world::WorldGenerator;

        Shader *shader;

        Shader *groupShader;

        world::WorldScene *worldScene;

        ::render::Texture *distortMap;

        WorldTileModel();

        void RenderSnow();

        void RenderPoliticalOverlay();

        void RenderInterestMap();

        void RenderGlobalLight();

        void RenderTilesAdvanced();

        void RenderPlayerPath();

        void RenderImprovements();

        void RenderFogOfWar();

    public:
        void Initialize() override;

        void Enable();

        void Render();

        void CreateCamera();

        void HandleCharacterSelected();

        Color GetGlobalLightColor();

        Rectangle GetFrustum();
    };
}