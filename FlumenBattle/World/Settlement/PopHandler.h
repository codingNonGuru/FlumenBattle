#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/Types.hpp"

struct Race;

namespace world::settlement
{
    class Settlement;
    class PopExtraData;
    class Cohort;
    class AfflictionPerformer;

    struct RaceGroup
    {
        RaceTypes Race;

        int Size;

        bool operator == (const RaceTypes &other) {return this->Race == other;}

        operator int() {return this->Size;}
    };

    struct Need
    {
        ResourceTypes Type;

        bool DoesDetermineHappiness;

        bool IsContinuous;

        int SatisfactionThreshold;

        int SatisfactionBoostPerConsumption;

        int Satisfaction {0};

        int GeneratedHappiness {0};

        bool IsMet {false};

        int Order {0};

        bool operator ==(const ResourceTypes otherType) {return otherType == Type;}
    };

    class PopHandler
    {
        friend class PopAllocator;

        friend class AfflictionPerformer;

        Settlement *settlement;

        int population;

        int highestPopulationEver;

        int growth;

        int currentHealth;

        int timeSinceAbandonment;

        int timeSinceRuined;

        int abandonmentSeverity;

        container::Array <Need> needs;
        
        int happiness;

        PopExtraData *extraData;

        bool isContent;

        bool isHappy;

        bool isEcstatic;

        container::Array <RaceGroup> raceGroups;

        const container::Array <RaceGroup> &GetNeighbourRaces();

        void RegenerateHealth();

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

        void IncreasePopulation(Settlement *, const container::Array <RaceGroup> * = nullptr);

        void KillPopulation();

        void CheckAbandonment(Settlement *);

        void UpdateGrowth(Settlement *);

        bool HasExtraData() const {return extraData != nullptr;}

        Settlement *GetSettlement() const {return settlement;}

        const container::Pool <Cohort> &GetCohorts() const;

        const container::Array <RaceGroup> &GetRaces() const {return raceGroups;}

        const Race *GetMostPopulousRace();

        int GetPotentialMidtermConsumption(ResourceTypes) const;
    };

    class PopAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Need> &allocator, container::ArrayAllocator <RaceGroup> &raceAllocator, PopHandler &handler) 
        {
            handler.needs.Initialize(allocator);

            handler.raceGroups.Initialize(raceAllocator);
        }

        static void AllocateExtraData(container::Pool <PopExtraData> &, container::PoolAllocator <Cohort> &, PopHandler &);

        static void Free(container::ArrayAllocator <Need> &allocator, container::ArrayAllocator <RaceGroup> &raceAllocator, PopHandler &handler) 
        {
            handler.needs.Terminate(allocator);

            handler.raceGroups.Terminate(raceAllocator);
        }

        static void FreeExtraData(container::Pool <PopExtraData> &, container::PoolAllocator <Cohort> &, PopHandler &);
    };
}