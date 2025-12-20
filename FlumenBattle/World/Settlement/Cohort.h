#pragma once

#include "FlumenBattle/Race.h"

namespace world::settlement
{
    struct Cohort
    {
        const Race *Race;

        int Health;

        int Experience;

    public:
        void Initialize(const struct Race *);
    };
}