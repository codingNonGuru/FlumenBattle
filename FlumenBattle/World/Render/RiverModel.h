#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Render/RiverRenderData.h"

namespace world::render
{ 
    class RiverModel : public core::Singleton <RiverModel>
    {
        friend class RendererAllocator;

        DataBuffer *buffer;

        container::Array <RiverRenderData> data;

    public:
        void Initialize();

        void Render();
    };
}