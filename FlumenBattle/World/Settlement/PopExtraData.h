#pragma once

#include "FlumenCore/Conventions.hpp"

enum class RaceTypes;

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

        void AddPopulation(RaceTypes);

        RaceTypes KillRandomPopulation();

        const container::Pool <Cohort> &GetCohorts() const {return cohorts;}
    };
}