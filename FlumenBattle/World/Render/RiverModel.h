#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    struct RiverRenderData
    {
        Float2 Positions[3];

        float ColorFactors[2];

        float Thicknesses[3];

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