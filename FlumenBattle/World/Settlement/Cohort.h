#pragma once

#include "FlumenBattle/Race.h"

namespace world::settlement
{
    struct Cohort
    {
        const Race *Race;

        int Health;

        int Experience;

        bool IsHired;

    public:
        void Initialize(const struct Race *);

        static const int MAXIMUM_HEALTH = 100;
    };
}