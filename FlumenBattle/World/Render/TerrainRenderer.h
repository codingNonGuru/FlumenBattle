#pragma once

#include "FlumenCore/Singleton.h"

namespace render
{
    class Texture;
}

namespace world
{
    class WorldGenerator;
}

namespace world::render 
{
    class TerrainRenderer : public core::Singleton <TerrainRenderer>
    {
        friend class world::WorldGenerator;

        ::render::Texture *distortMap;

        void RenderLandTilesToDiffuseStencil();

        void SharpenDiffuseStencil(Color);

        void RenderSteppeTilesToDiffuseStencil();

        void RenderSeaTilesToScreen();

        void RenderLandTilesToScreen(float);

    public:
        TerrainRenderer();

        void Initialize();

        void Render();
    };
}