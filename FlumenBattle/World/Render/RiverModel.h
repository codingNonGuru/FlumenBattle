#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    struct RiverRenderData
    {
        Float4 Color;

        Float2 Positions[3];

        float Thickness;

        float Padding[1];
    };

    class RiverModel : public core::Singleton <RiverModel>
    {
        DataBuffer *buffer;

        container::Array <RiverRenderData> data;

    public:
        void Initialize();

        void Render();
    };
}