#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    struct RoadRenderData
    {
        Float4 Color;
        
        Float2 Position;

        float Rotation;

        float Length;

        float Thickness;

        float Padding[3];
    };

    class RoadModel : public core::Singleton <RoadModel>
    {
        friend class RendererAllocator;

        DataBuffer *buffer;

        /*DataBuffer *colorBuffer;

        DataBuffer *rotationBuffer;

        DataBuffer *lengthBuffer;

        DataBuffer *thicknessBuffer;*/

        DataBuffer *tileQueueBuffer;

        container::Array <RoadRenderData> data;

        /*container::Array <Float4> colors;

        container::Array <Float> rotations;

        container::Array <Float> lengths;

        container::Array <Float> thicknesses;*/

        container::Array <unsigned int> tileQueue;

        void UpdateData();

    public:
        void Initialize();

        void Render();
    };
}