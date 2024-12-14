#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

    struct Need
    {
        ResourceTypes Type;

        int Satisfaction {0};

        bool IsMet {false};

        bool operator ==(const ResourceTypes otherType) {return otherType == Type;}
    };

    class PopHandler
    {
        friend class PopAllocator;

        container::Array <Need> needs;

    public:
        void Initialize();

        void PlaceOrders(Settlement &);

        void UpdateNeeds(Settlement &);

        int GetNeedSatisfaction(ResourceTypes resource) const {return needs.Find(resource)->Satisfaction;}
    };

    class PopAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Initialize(allocator);
        }

        static void Free(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Terminate(allocator);
        }
    };
}