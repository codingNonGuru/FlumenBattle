#pragma once

#include "FlumenCore/Singleton.h"

class DataBuffer;
class FrameBuffer;

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
    struct PerlinData
    {
        ::render::Texture *Map;

        float MixStrength;

        float Granularity;
    };

    class TerrainRenderer : public core::Singleton <TerrainRenderer>
    {
        friend class world::WorldGenerator;

        ::render::Texture *distortMap;

        ::render::Texture *blotchyMap;

        void ClearStencilBuffer(Color);

        void RenderHexesToDiffuseStencil(DataBuffer *, Color, float);

        void SharpenDiffuseStencil(FrameBuffer *, FrameBuffer *, Color, Range, PerlinData);

        void DodgeStencil(FrameBuffer *, FrameBuffer *, float, Position2);

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