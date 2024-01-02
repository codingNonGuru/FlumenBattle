#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Settlement/Shipment.h"
#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    class Polity;

    namespace group
    {
        class GroupDynamics;
    }

    namespace disaster
    {
        class Earthquake;
    }
}

namespace world::settlement 
{
    struct Affliction;
    struct Event;
    struct EventGenerator;
    class SettlementProduction;
    class ConditionManager;
    struct ConditionData;
    class BuildingManager;
    class Path;

    struct SettlementTile
    {
        world::WorldTile * Tile;

        bool IsWorked;

        bool IsBuilt;
    };

    struct Link
    {
        Path *Path;

        bool HasShipped {false};

        bool operator== (const settlement::Path &path) const;
    };

    class Settlement
    {
        friend class world::Polity;

        friend class world::WorldScene;

        friend class AfflictionPerformer;

        friend class SettlementFactory;

        friend class SettlementAllocator;

        friend struct EventGenerator;

        friend class SettlementProductionFinisher;

        friend class SettlementLabel;

        friend class BuildingDamager;

        Word name;

        Color banner;

        world::WorldTile *location;

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer foodStorage;

        Integer foodProduction;

        Integer foodBalance;

        FoodSecurity foodSecurity;

        Integer metalStorage;

        Integer metalProduction;

        Integer metalBalance;

        Integer growth;

        Integer cultureGrowth;

        Pool <SettlementTile> tiles;

        world::Polity *polity;

        Pool <Affliction> afflictions;

        Pool <Event> events;

        Pool <Link> links;

        SettlementProduction *currentProduction;

        ModifierManager modifierManager;

        ConditionManager *conditionManager;
        
        BuildingManager *buildingManager;

        Shipment shipment;

        Integer lastShipmentTime;

        void Initialize(Word, Color, world::WorldTile *);

        world::WorldTile * FindColonySpot();

        void WorkNewTile();

        void GrowBorders();

        void DecideProduction();

    public:
        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        world::WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        void UpdateFoodSituation();

        void UpdateMetalSituation();

        Integer GetFoodProduction() const;

        Integer GetFoodStorage() const {return foodStorage;}

        Integer GetMetalStorage() const {return metalStorage;}

        Integer GetMetalProduction() const;

        Integer GetIndustrialProduction() const;

        Integer GetScienceProduction() const;

        Integer GetPopulation() const {return population;}

        Integer GetGrowth() const {return growth;}

        Integer GetWorkedTiles() const;

        world::Polity *GetPolity() const {return polity;}

        int GetModifier(Modifiers) const;

        void SetPolity(world::Polity *);

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        void StrengthenPatrol();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();

        void PrepareTransport();

        void SendTransport();

        void ReceiveTransport();

        void AddPath(Path *path);

        Path *GetPathTo(Settlement *settlement);
    };
}