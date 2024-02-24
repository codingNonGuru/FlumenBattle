#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::polity
{
    class Faction;
    class Polity;

    class PolityAllocator : public core::Singleton <PolityAllocator>
    {
        container::Pool <Polity>::Memory polityMemory;

        container::PoolAllocator <settlement::Settlement *>::Memory politySettlementMemory;

        container::PoolAllocator <settlement::Settlement *>::Memory factionSettlementMemory;

        container::PoolAllocator <Faction>::Memory factionMemory;


        container::Pool <Polity> polities;

        container::PoolAllocator <settlement::Settlement *> politySettlementAllocator;

        container::PoolAllocator <settlement::Settlement *> factionSettlementAllocator;

        container::PoolAllocator <Faction> factionAllocator;

    public:
        void AllocateWorldMemory(int);

        void PreallocateMaximumMemory();

        container::Pool <Polity> *GetPolities() {return &polities;}

        Polity *AllocatePolity();

        Faction *AllocateFaction(Polity *);

        void FreePolity(Polity *);

        void FreeFaction(Polity *, Faction *);
    };
}