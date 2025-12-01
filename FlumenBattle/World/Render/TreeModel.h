#pragma once

#include "FlumenCore/Singleton.h"

class DataBuffer;

namespace world::render
{
    class TreeModel : public core::Singleton <TreeModel>
    {
        friend class RendererAllocator;

        void PrepareQueue();

        DataBuffer *positionBuffer;

        DataBuffer *colorBuffer;

        DataBuffer *scaleBuffer;

        DataBuffer *tileQueueBuffer;

        container::Array <Position2> positions;

        container::Array <Float4> colors;

        container::Array <Float> scales;

        container::Array <unsigned int> tileQueue;

    public:
        void Initialize();

        void RenderShadows();

        void Render();

        static int GetMaximumTreesPerTile();
    };
}