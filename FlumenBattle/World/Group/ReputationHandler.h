#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Settlement;
    enum class SettlementAttitudes;
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

        settlement::SettlementAttitudes GetAttitude(settlement::Settlement *) const;

        const container::Pool <Reputation> &GetReputations() const {return reputations;}
    };
}