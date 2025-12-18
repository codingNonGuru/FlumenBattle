#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Render/RiverRenderData.h"

namespace render
{
    class Texture;
}

namespace world
{
    class RiverGenerator;
}

namespace world::render
{ 
    class RiverModel : public core::Singleton <RiverModel>
    {
        friend class RendererAllocator;

        friend class world::RiverGenerator;

        DataBuffer *buffer;
        
        container::Array <RiverRenderData> data;

        ::render::Texture *angleTexture;

        ::render::Texture *radiusTexture;

    public:
        void Initialize();

        void Render();
    };
}