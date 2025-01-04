#pragma once

#include "FlumenCore/Container/Array.hpp"

namespace world
{
    class WorldAllocator;

    namespace settlement
    {
        class Settlement;
    }
}

namespace world::tile
{
    struct Ruin;

    class RuinHandler
    {
        friend class RuinAllocator;

        container::Array <Ruin> ruins;

    public:
        void Add(settlement::Settlement *, bool);

        int GetCount() const {return ruins.GetSize();}
    };

    class RuinAllocator
    {
        friend class world::WorldAllocator;

        static void Allocate(container::ArrayAllocator <Ruin> &allocator, RuinHandler &handler);
    };
}