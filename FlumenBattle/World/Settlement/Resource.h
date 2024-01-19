#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Singleton.h"

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;
    enum class AbundanceLevels;

    struct ResourceType
    {
        ResourceTypes Type;

        int Value;

        int PopulationConsumption {0};

        bool DoesImprovementWorkAnywhere {false};

        ResourceType(ResourceTypes type, int value, bool doesImprovement) : Type(type), Value(value), DoesImprovementWorkAnywhere(doesImprovement) {}

        ResourceType(ResourceTypes type, int value) : Type(type), Value(value) {}
    };

    struct Resource
    {
        const ResourceType *Type;

        int Storage {100};

        int Production {0};

        int Balance {0};

        AbundanceLevels Abundance;

        void Update(Settlement &);

        int GetProduction(Settlement &);

        bool operator==(ResourceTypes type) {return Type->Type == type;}
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

        void Initialize();

        void Update(Settlement &);

        Resource *Get(ResourceTypes) const;
    };

    /*class ResourceFactory : public core::Singleton <ResourceFactory>
    {
        friend class ResourceManager;

        Resource Create(ResourceTypes);
    };*/

    class ResourceAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Resource> &allocator, ResourceHandler &handler) 
        {
            handler.resources.Initialize(allocator);
        }

        static void Free(container::ArrayAllocator <Resource> &allocator, ResourceHandler &handler) 
        {
            handler.resources.Terminate(allocator);
        }
    };
}