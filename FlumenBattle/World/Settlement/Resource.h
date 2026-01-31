#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Settlement/Types.h"
#include "JobSet.h"

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;
    enum class AbundanceLevels;
    class JobSet;

    struct ResourceType
    {
        struct Throughput
        {
            ResourceTypes Resource;

            int Amount;
        };

        ResourceTypes Type;

        Word Name;

        Word TextureName;

        int Value;

        container::Array <Modifiers> RelatedModifiers;

        container::Array <Throughput> InputResources {0};
        
        int OutputAmount {1};

        //Throughput OutputResource {ResourceTypes::NONE, 0};

        int PopulationConsumption {0};

        bool DoesImprovementWorkAnywhere {false};

        bool IsProductionTileBased {false};

        ResourceType(ResourceTypes type, Word name, Word textureName, int value, bool doesImprovement) : Type(type), Name(name), TextureName(textureName), Value(value), DoesImprovementWorkAnywhere(doesImprovement) {}

        ResourceType(ResourceTypes type, Word name, Word textureName, int value) : Type(type), Name(name), TextureName(textureName), Value(value) {}
    };

    struct Resource
    {
        const ResourceType *Type;

        int Storage {100};

        int Production {0};

        int Balance {0};

        int Order {0};

        AbundanceLevels ShortTermAbundance;

        AbundanceLevels LongTermAbundance;

        bool HasPopulationOrdered {false};

        int AbundanceDegree;

        int ScarcityDegree;

        bool CanFulfillOrders;

        int GetPotentialProduction(const Settlement &) const;

        int GetProductionFromTiles(const Settlement &) const;

        int GetProductionFromBuildings(const Settlement &) const;

        void ResetOrder() {Order = 0;}

        void PlaceOrders(const Settlement &);

        void CheckOrderFullfilment();

        void ExecuteOrders(const Settlement &);

        void FinishProduction();

        void UpdateAbundance(Settlement &);

        void UpdateStorage(Settlement &);

        bool operator==(ResourceTypes type) {return Type->Type == type;}

        bool HasRelatedModifier(Modifiers modifier) const {return Type->RelatedModifiers.Find(modifier) != nullptr;}
    };

    class ResourceSet
    {
    protected:
        container::Array <Resource> resources;

        ResourceSet() {}
    };

    class ResourceHandler : protected ResourceSet
    {
        friend class ResourceAllocator;

        friend class Settlement;

        //int GetAmount(ResourceTypes);

        void Initialize(const Settlement *);

        void ResetOrders();

        void PlaceOrders(Settlement &);

        void ExecuteOrders(Settlement &);

        void Update(Settlement &);

        JobSet jobSet;

        const Settlement *parent;

    public:
        Resource *Get(ResourceTypes) const;

        const Settlement *GetParent() const {return parent;}
    };

    class ResourceFactory : public core::Singleton <ResourceFactory>
    {
    public:
        const ResourceType *CreateType(ResourceTypes);
    };

    class ResourceAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Resource> &allocator, ResourceHandler &handler) 
        {
            handler.resources.Initialize(allocator);
        }

        static void AllocateExtraData(container::PoolAllocator <Job> &allocator, ResourceHandler &handler);

        static void Free(container::ArrayAllocator <Resource> &allocator, ResourceHandler &handler) 
        {
            handler.resources.Terminate(allocator);
        }

        static void FreeExtraData(container::PoolAllocator <Job> &allocator, ResourceHandler &handler);
    };
}