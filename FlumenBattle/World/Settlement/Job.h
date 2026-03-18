#pragma once

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Cohort;
    class ResourceHandler;
    struct SettlementTile;
    class Settlement;

    enum class JobStatus {OBTAINING_MATERIALS, PRODUCING, DELIVERING_GOODS};

    class Job
    {
        ResourceTypes resourceType;

        SettlementTile *tile;

        Cohort *cohort;

        int progress;

        JobStatus status;

        bool isUsingBuilding;

        int finishedCycleCount;

        int timeSpent;

        bool canProcureMaterials;

    public:
        void Initialize(Cohort *, ResourceTypes);

        void Initialize(Cohort *, SettlementTile *);

        void PlaceOrders(ResourceHandler &);

        void CheckMaterialAvailability(ResourceHandler &);

        void ExecuteOrders(ResourceHandler &);

        void FinishProduction(ResourceHandler &);

        void UseBuilding() {isUsingBuilding = true;}

        void QuitBuilding() {isUsingBuilding = false;}

        ResourceTypes GetResource() const {return resourceType;}

        Cohort *GetCohort() const {return cohort;}

        SettlementTile *GetTile() const {return tile;}

        int GetProgress() const {return progress;}

        JobStatus GetStatus() const {return status;}

        int GetOutput(const Settlement *settlement) const;

        int GetOutput(ResourceTypes, const Settlement *settlement) const;

        bool IsUsingBuilding() const {return isUsingBuilding;}

        int GetTimeSpent() const {return timeSpent;}

        int GetCycleCount() const {return finishedCycleCount;}

        bool CanProcureMaterials() const {return canProcureMaterials;}

        bool operator == (const Cohort *cohort) {return this->cohort == cohort;}
    };
}