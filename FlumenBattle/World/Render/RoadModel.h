#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    struct RoadRenderData
    {
        Float4 Color;

        Float2 Positions[3];

        float Thickness;

        float Padding[1];
    };

    class RoadModel : public core::Singleton <RoadModel>
    {
        friend class RendererAllocator;

        DataBuffer *buffer;

        DataBuffer *tileQueueBuffer;

        container::Array <RoadRenderData> data;

        container::Array <unsigned int> tileQueue;

        void UpdateData();

    public:
        void Initialize();

        void Render();
    };
}