#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::render
{
    struct RiverRenderData
    {
        Float2 Positions[3];

        float ColorFactors[2];

        float Thicknesses[2];
    };
}