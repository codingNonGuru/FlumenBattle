#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class RoadModel : public core::Singleton <RoadModel>
    {
        friend class RendererAllocator;

        DataBuffer *positionBuffer;

        DataBuffer *colorBuffer;

        DataBuffer *rotationBuffer;

        DataBuffer *lengthBuffer;

        DataBuffer *thicknessBuffer;

        DataBuffer *tileQueueBuffer;

        container::Array <Position2> positions;

        container::Array <Float4> colors;

        container::Array <Float> rotations;

        container::Array <Float> lengths;

        container::Array <Float> thicknesses;

        container::Array <unsigned int> tileQueue;

        void UpdateData();

    public:
        void Initialize();

        void Render();
    };
}