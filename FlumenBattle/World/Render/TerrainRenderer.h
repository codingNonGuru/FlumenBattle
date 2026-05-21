#pragma once

#include "FlumenCore/Singleton.h"

class DataBuffer;

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

        void ClearStencilBuffer(Color);

        void RenderHexesToDiffuseStencil(DataBuffer *, Color, float);

        void SharpenDiffuseStencil(Color);

        void RenderSteppeTilesToDiffuseStencil();

        void RenderSeaTilesToScreen();

        void RenderCornersToScreen();

        void RenderTextureToScreen(::render::Texture *, float);

    public:
        TerrainRenderer();

        void Initialize();

        void Render();
    };
}