#pragma once

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Cohort;
    class ResourceHandler;
    struct SettlementTile;

    class Job
    {
        enum class JobStatus {OBTAINING_MATERIALS, PRODUCING, DELIVERING_GOODS};

        ResourceTypes resourceType;

        SettlementTile *tile;

        Cohort *cohort;

        int progress;

        JobStatus status;

    public:
        void Initialize(Cohort *, ResourceTypes);

        void Initialize(Cohort *, SettlementTile *);

        void PlaceOrders(ResourceHandler &);

        void ExecuteOrders(ResourceHandler &);

        void FinishProduction(ResourceHandler &, bool);

        ResourceTypes GetResource() const {return resourceType;}

        Cohort *GetCohort() const {return cohort;}

        SettlementTile *GetTile() const {return tile;}
    };
}