#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

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

        int population;

        int growth;

        int growthThreshold {1000};

        int timeSinceAbandonment;

        int timeSinceRuined;

        int abandonmentSeverity;

        container::Array <Need> needs;
        
        int happiness;

        bool isContent;

        bool isHappy;

        bool isEcstatic;

    public:
        void Initialize();

        void PlaceOrders(Settlement &);

        void UpdateNeeds(Settlement &);

        int GetNeedSatisfaction(ResourceTypes resource) const {return needs.Find(resource)->Satisfaction;}

        bool IsContent() const {return isContent;}

        bool IsHappy() const {return isHappy;}

        bool IsEcstatic() const {return isEcstatic;}

        float GetHappinessRatio() const;

        int GetPopulation() const;

        int GetGrowth() const {return growth;}

        float GetGrowthRatio() const {return float(growth) / float(growthThreshold);}

        bool IsSettlementAbandoned() const;

        bool IsSettlementRuins() const;

        bool IsSettlementCompletelyGone() const;

        int GetAbandonmentSeverity() const;

        void IncreasePopulation(Settlement *);

        void KillPopulation();

        void CheckAbandonment(Settlement *);

        void UpdateGrowth(Settlement *);
    };

    class PopAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Initialize(allocator);
        }

        static void Free(container::ArrayAllocator <Need> &allocator, PopHandler &handler) 
        {
            handler.needs.Terminate(allocator);
        }
    };
}