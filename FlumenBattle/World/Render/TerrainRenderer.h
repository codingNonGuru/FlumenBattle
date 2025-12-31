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

        void SharpenDiffuseStencil(Float3);

        void RenderSteppeTilesToDiffuseStencil();

        void RenderSeaTilesToScreen();

        void RenderLandTilesToScreen();

        void RenderSteppeTilesToScreen();

    public:
        TerrainRenderer();

        void Initialize();

        void Render();
    };
}