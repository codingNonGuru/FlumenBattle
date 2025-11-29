#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::tile
{
    struct WorldEdge;

    class River
    {
        container::SmartBlock <WorldEdge *, 64> segments;

        River() {}

    public:
        void Initialize();

        void AddSegment(WorldEdge *);

        container::SmartBlock <WorldEdge *, 64> &GetSegments() {return segments;}
    };
}