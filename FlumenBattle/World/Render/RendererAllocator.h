#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Render/FarmData.h"

namespace world::render
{
    struct FoamSegmentData;

    class RendererAllocator : public core::Singleton <RendererAllocator>
    {
        friend class core::Singleton <RendererAllocator>;

        container::Array <Position2>::Memory farmPositionMemory;

        container::Array <Position2>::Memory farmOffsetMemory;

        container::Array <Float>::Memory farmRotationMemory;

        container::Grid <FarmData>::Memory farmDataMemory;


        container::Array <FoamSegmentData>::Memory foamSegmentMemory;


        container::Array <Position2>::Memory treePositionMemory;

        container::Array <Float4>::Memory treeColorMemory;

        container::Array <Float>::Memory treeScaleMemory;

        container::Array <unsigned int>::Memory treeQueueMemory;


        container::Array <Position2>::Memory mountainPositionMemory;

        container::Array <Float4>::Memory mountainColorMemory;

        container::Array <Float>::Memory mountainScaleMemory;

        container::Array <unsigned int>::Memory mountainQueueMemory;

        container::Array <int>::Memory mountainTypeMemory;


        container::Array <Position2>::Memory roadPositionMemory;

        container::Array <Float4>::Memory roadColorMemory;

        container::Array <Float>::Memory roadRotationMemory;

        container::Array <Float>::Memory roadLengthMemory;

        container::Array <Float>::Memory roadThicknessMemory;

        container::Array <unsigned int>::Memory roadQueueMemory;

        RendererAllocator();

    public:
        void Allocate(int size);
    };
}