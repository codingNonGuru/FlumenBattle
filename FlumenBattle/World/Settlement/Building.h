#pragma once

#include "FlumenCore/Container/Pool.hpp"

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

    struct BuildingType
    {
        BuildingTypes Type;

        int Cost;

        explicit BuildingType(BuildingTypes type, int cost) : Type(type), Cost(cost) {}

        virtual void HandleApplyEffect(Settlement &) = 0;
    };

    class Building
    {
        const BuildingType *type;

        bool isDamaged;

        bool isTall;

    public:
        Building() : type(nullptr), isDamaged(false), isTall(false) {}

        Building(const BuildingType *type, bool isDamaged, bool isTall) : 
        type(type), 
        isDamaged(isDamaged), 
        isTall(isTall) {} 

        void ApplyEffect(Settlement &) const;

        bool operator ==(BuildingTypes buildingType) const {return this->type->Type == buildingType;}
    };

    class BuildingSet
    {
        friend class BuildingSetAllocator;

        friend class BuildingManager;

        container::Pool <Building> buildings; 

        BuildingSet() {}

        BuildingSet& operator=(const BuildingSet &) = delete;

        container::Pool <Building> &Get() {return &buildings;}
    };

    class BuildingManager
    {
        BuildingSet buildingSet;

        friend class Settlement;

        friend class BuildingSetAllocator;

        bool HasBuilding(BuildingTypes) const;

        void AddBuilding(BuildingTypes);

        void RemoveBuilding(BuildingTypes);

        void ApplyModifiers(Settlement &) const;

        void Update();
    };

    class BuildingSetAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::PoolAllocator <Building> &allocator, BuildingManager &manager) 
        {
            manager.buildingSet.buildings.Initialize(allocator);
        }
    };

    class BuildingFactory : public Singleton <BuildingFactory>
    {
        friend class BuildingManager;

        Building Create(BuildingTypes);
    };
}