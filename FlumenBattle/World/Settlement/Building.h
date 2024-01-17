#pragma once

#include "FlumenCore/Container/Pool.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::disaster
{
    class Earthquake;
}

namespace world::settlement
{
    class Settlement;

    struct BuildingType
    {
        BuildingTypes Type;

        int Cost;

        explicit BuildingType(BuildingTypes type, int cost) : Type(type), Cost(cost) {}

        virtual void HandleApplyEffect(Settlement &) const = 0;
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

        bool IsTall() const {return isTall;}
    };

    class BuildingSet
    {
        friend class BuildingSetAllocator;

        friend class BuildingManager;

        friend class BuildingDamager;

        container::Pool <Building> buildings; 

        BuildingSet() {}

        BuildingSet& operator=(const BuildingSet &) = delete;

        const container::Pool <Building> &Get() const {return buildings;}
    };

    class BuildingManager
    {
        BuildingSet buildingSet;

        Settlement *settlement;

        friend class Settlement;

        friend class BuildingSetAllocator;

        friend class BuildingDamager;

        void Initialize(Settlement *);

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

    class BuildingDamager
    {
        friend class Settlement;

        static void DamageImprovements(const disaster::Earthquake &earthquake, Settlement &settlement);

        static void DamageBuildings(const disaster::Earthquake &earthquake, BuildingManager &buildingManager);
    };

    class BuildingFactory : public core::Singleton <BuildingFactory>
    {
        friend class BuildingManager;

        Building Create(BuildingTypes);
    };
}