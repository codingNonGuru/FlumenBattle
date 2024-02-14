#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Settlement;
}

namespace world::group
{
    typedef int Hour;

    struct ReputationFactor
    {
        Hour TimeAcquired;

        int BaseStrength;

        bool HasExpired() const;

        int GetStrength() const;
    };

    struct Reputation
    {
        settlement::Settlement *Settlement {nullptr};

        container::Pool <ReputationFactor> Factors {8};

        bool operator==(settlement::Settlement * const &settlement) {return settlement == Settlement;}

        int GetSum() const;

        void Update();
    };
}