#pragma once

#include "FlumenCore/Container/Array.hpp"

namespace world::polity
{
    struct Neighbor;
    class PolityAllocator;
    class Polity;

    class NeighborHandler
    {
        friend class NeighborAllocator;

        friend class Polity;

        container::Array <Neighbor> neighbors;

    public:
        void CalculateNeighbors(Polity *);
    };

    class NeighborAllocator
    {
        friend class PolityAllocator;

        static void Allocate(container::ArrayAllocator <Neighbor> &, NeighborHandler &);

        static void Free(container::ArrayAllocator <Neighbor> &, NeighborHandler &);
    };
}