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

        RendererAllocator();

    public:
        void Allocate(int size);
    };
}