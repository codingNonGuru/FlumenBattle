#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::tile
{
    struct WorldEdge;

    struct RiverTwist
    {
        WorldEdge *First;

        WorldEdge *Second;

        Position2 Position;
    };

    class River
    {
        container::SmartBlock <WorldEdge *, 128> segments;

        container::SmartBlock <RiverTwist, 128> twists;

        River() {}

    public:
        void Initialize();

        void AddSegment(WorldEdge *);

        RiverTwist *AddTwist();

        container::SmartBlock <WorldEdge *, 128> &GetSegments() {return segments;}

        container::SmartBlock <RiverTwist, 128> &GetTwists() {return twists;}
    };
}