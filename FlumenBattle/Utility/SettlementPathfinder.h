#pragma once

#include <chrono>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Graph.h"
#include "FlumenCore/Conventions.hpp"

using namespace std::chrono;

namespace world
{
    namespace settlement
    {
        class Settlement;
    }
}

namespace utility
{
    class SettlementPathfinder : public core::Singleton <SettlementPathfinder>
    {
        friend class core::Singleton <SettlementPathfinder>;
        
    public:
        struct SettlementData
        {
            world::settlement::Settlement *Settlement;

            Integer Distance;

            Integer Complexity;

            bool IsFringe;

            SettlementData() {}

            SettlementData(world::settlement::Settlement *settlement) : Settlement(settlement) {}

            SettlementData(world::settlement::Settlement *settlement, Integer distance) : Settlement(settlement), Distance(distance), IsFringe(false) {}
        };

        struct SettlementPathData
        {
            const Array <SettlementData> &Settlements;

            Integer Complexity;

            Integer Length;
        };

    private:
        typedef container::Graph <SettlementData, 8> SettlementGraph;

        Array <SettlementData> visitedSettlements;

        SettlementGraph settlementPaths;

        SettlementPathfinder()
        {
            visitedSettlements = Array <SettlementData>(256);
            settlementPaths = SettlementGraph(4096);
        }

    public:
        SettlementPathData FindPathToSettlement(world::settlement::Settlement *, world::settlement::Settlement *);
    };
}