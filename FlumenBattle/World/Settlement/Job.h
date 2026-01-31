#pragma once

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Cohort;
    class ResourceHandler;

    class Job
    {
        enum class JobStatus {OBTAINING_MATERIALS, PRODUCING, DELIVERING_GOODS};

        ResourceTypes resourceType;

        Cohort *cohort;

        int progress;

        JobStatus status;

    public:
        void Initialize(Cohort *, ResourceTypes);

        void PlaceOrders(ResourceHandler &);

        void ExecuteOrders(ResourceHandler &);

        void FinishProduction(ResourceHandler &);
    };
}