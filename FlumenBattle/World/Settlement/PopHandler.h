#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;
    class PopExtraData;
    class Cohort;

    struct Need
    {
        ResourceTypes Type;

        bool DoesDetermineHappiness;

        bool IsContinuous;

        int SatisfactionThreshold;

        int Satisfaction {0};

        int GeneratedHappiness {0};

        bool IsMet {false};

        bool operator ==(const ResourceTypes otherType) {return otherType == Type;}
    };

    class PopHandler
    {
        friend class PopAllocator;

        Settlement *settlement;

        int population;

        int highestPopulationEver;

        int growth;

        int timeSinceAbandonment;

        int timeSinceRuined;

        int abandonmentSeverity;

        container::Array <Need> needs;
        
        int happiness;

        PopExtraData *extraData;

        bool isContent;

        bool isHappy;

        bool isEcstatic;

    public:
        void Initialize(Settlement *);

        void Deepen();

        void PlaceOrders(Settlement &);

        void UpdateNeeds(Settlement &);

        int GetNeedSatisfaction(ResourceTypes resource) const {return needs.Find(resource)->Satisfaction;}

        bool IsContent() const {return isContent;}

        bool IsHappy() const {return isHappy;}

        bool IsEcstatic() const {return isEcstatic;}

        float GetHappinessRatio() const;

        int GetPopulation() const;

        int GetHighestPopulationEver() const {return highestPopulationEver;}

        int GetGrowth() const {return growth;}

        float GetGrowthRatio() const;

        bool IsSettlementAbandoned() const;

        bool IsSettlementRuins() const;

        bool IsSettlementCompletelyGone() const;

        int GetAbandonmentSeverity() const;

        void IncreasePopulation(Settlement *);

        void KillPopulation();

        void CheckAbandonment(Settlement *);

        void UpdateGrowth(Settlement *);

        bool HasExtraData() const {return extraData != nullptr;}

        Settlement *GetSettlement() const {return settlement;}

        const container::Pool <Cohort> &GetCohorts() const;
    };

    class PopAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Initialize(allocator);
        }

        static void AllocateExtraData(container::Pool <PopExtraData> &, container::PoolAllocator <Cohort> &, PopHandler &);

        static void Free(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Terminate(allocator);
        }

        static void FreeExtraData(container::Pool <PopExtraData> &, container::PoolAllocator <Cohort> &, PopHandler &);
    };
}