#pragma once

#include "FlumenBattle/Race.h"

namespace world::settlement
{
    class Job;

    struct Cohort
    {
        const Race *Race;

        int Health;

        int Experience;

        bool IsHired;

        Job *Job;

    public:
        void Initialize(const struct Race *);

        static const int MAXIMUM_HEALTH = 100;
    };
}