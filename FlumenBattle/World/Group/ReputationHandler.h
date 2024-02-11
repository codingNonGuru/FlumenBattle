#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Settlement;
}

namespace world::group
{
    struct Reputation;

    class ReputationHandler
    {
        container::Pool <Reputation> reputations;

    public:
        ReputationHandler();

        void AddFactor(settlement::Settlement *, int);

        int GetReputation(settlement::Settlement *) const;
    };
}