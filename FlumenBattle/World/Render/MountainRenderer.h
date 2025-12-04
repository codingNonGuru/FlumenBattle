#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class MountainRenderer : public core::Singleton <MountainRenderer>
    {
        friend class RendererAllocator;

        DataBuffer *positionBuffer;

        DataBuffer *colorBuffer;

        DataBuffer *scaleBuffer;

        DataBuffer *tileQueueBuffer;

        DataBuffer *typeBuffer;

        container::Array <Position2> positions;

        container::Array <Float4> colors;

        container::Array <Float> scales;

        container::Array <unsigned int> tileQueue;

        container::Array <int> types;

        void PrepareQueue();

    public:
        void Initialize();

        void Render();

        static int GetMaximumMountainsPerTile();
    };
}