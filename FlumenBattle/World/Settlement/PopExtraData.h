#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::settlement
{
    class Cohort;
    class PopHandler;

    class PopExtraData
    {
        friend class PopAllocator;

        container::Pool <Cohort> cohorts;

        PopHandler *popHandler;

    public:
        void Initialize(PopHandler *);

        const container::Pool <Cohort> &GetCohorts() const {return cohorts;}
    };
}