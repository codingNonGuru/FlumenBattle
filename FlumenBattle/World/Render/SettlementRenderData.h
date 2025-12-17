#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::render
{
    struct BuildingRenderData
    {
        Float4 Color;

        Float2 Position;

        Float2 Size;

        float Rotation;

        float Padding[3];
    };
}