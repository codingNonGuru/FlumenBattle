#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Render/Model.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Shader;

namespace world
{
    class WorldScene;
}

namespace world::render 
{
    class WorldTileModel : public Model, public core::Singleton <WorldTileModel>
    {
        friend class Singleton;

        Shader *shader;

        Shader *groupShader;

        world::WorldScene *worldScene;

        WorldTileModel();

        void RenderTiles();

        void RenderSnow();

        void RenderPaths();

        void RenderPoliticalOverlay();

        void RenderInterestMap();

        void RenderSettlements();

        void RenderGlobalLight();

        void RenderTilesAdvanced();

        void RenderPlayerPath();

        void RenderGroupSightings();

        void RenderImprovements();

        void RenderBorderExpansionMap();

        void RenderSettleModeMap();

        void RenderExploreMap();

        void RenderTileDevelopMap();

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