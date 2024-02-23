#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Settlement/Shipment.h"
#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    struct SimulationDomain;
    enum class SimulationLevels;

    namespace polity
    {
        class Polity;
        class Faction;
    }

    namespace group
    {
        class GroupDynamics;
        class Group;
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
    struct Condition;
    class Building;

    struct SettlementTile
    {
        world::WorldTile * Tile;

        bool IsWorked;

        bool IsBuilt;
    };

    struct Link
    {
        Path *Path;

        Settlement *Other;

        bool HasShipped {false};

        bool operator== (const settlement::Path &path) const;
    };

    class Settlement
    {
        friend class polity::Polity;

        friend class world::WorldScene;

        friend class AfflictionPerformer;

        friend class SettlementFactory;

        friend class SettlementAllocator;

        friend struct EventGenerator;

        friend class ProductionFinisher;

        friend class SettlementLabel;

        friend class BuildingDamager;

        friend struct Resource;

        Word name;

        Color banner;

        world::WorldTile *location;

        world::SimulationDomain *simulationDomain;

        SimulationLevels simulationLevel;

        int timeSinceSimulationChange;

        bool hasUsedSimulationChange {true};

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer growth;

        Integer growthThreshold {1000};

        Integer cultureGrowth;

        bool needsToReorganizeWork {true};

        bool areNearbyTilesTaken {false};

        bool hasAvailableColonySpots;

        Pool <SettlementTile> tiles;

        polity::Polity *polity;

        int distanceToCapital;

        Pool <Affliction> afflictions;

        Pool <Event> events;

        Pool <Link> links;

        SettlementProduction *currentProduction;

        ModifierManager modifierManager;

        ConditionManager *conditionManager;
        
        BuildingManager *buildingManager;

        ResourceHandler resourceHandler;

        int storage {400};

        Shipment shipment;

        Integer lastShipmentTime;

        Integer independenceDrive {0};

        polity::Faction *faction {nullptr};

        struct PathData
        {
            bool IsVisited = false;

            bool IsToBeVisited = false;
        } pathData;

        void Initialize(Word, Color, world::WorldTile *);

        void WorkNewTile();

        void GrowBorders();

    public:
        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        world::WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        Integer GetIndustrialProduction() const;

        Integer GetScienceProduction() const;

        Integer GetPopulation() const {return population;}

        Integer GetGrowth() const {return growth;}

        float GetGrowthRatio() const {return float(growth) / float(growthThreshold);}

        AbundanceLevels GetHousingAdequacy() const;

        Integer GetWorkedTiles() const;

        const group::GroupDynamics &GetGroupDynamics() const {return *groupDynamics;}

        polity::Polity *GetPolity() const {return polity;}

        Settlement *GetRuler() const;

        int GetModifier(Modifiers) const;

        Resource *GetResource(ResourceTypes type) const {return resourceHandler.Get(type);}

        int GetResourcePrice(ResourceTypes type) const {return resourceHandler.Get(type)->Type->Value;}

        int GetMulePrice() const;

        int GetStock(ResourceTypes type) const {return resourceHandler.Get(type)->Storage;}

        SettlementProduction *GetCurrentProduction() const {return currentProduction;}

        const container::Pool <Building> &GetBuildings() const;

        bool HasBuilding(BuildingTypes) const;

        int GetBuildingCount(BuildingTypes) const;

        polity::Faction *GetFaction() const {return faction;}

        world::SimulationLevels GetSimulationLevel() const;

        void SetupSimulation();

        world::WorldTile *FindColonySpot();

        void RemoveGroup(const group::Group &);

        void SetPolity(polity::Polity *);

        void SetFaction(polity::Faction *newFaction) {faction = newFaction;}

        void KillPopulation();

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        void StrengthenPatrol();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();

        void DecideProduction();

        void SetProduction(ProductionOptions);

        void UpdatePolitics();

        void UpdateDistanceToCapital();

        void PrepareTransport();

        void SendTransport();

        void ReceiveTransport();

        void AddPath(Path *path);

        void UpdateColonialMap();

        Path *GetPathTo(Settlement *settlement);

        const Pool <Link> &GetLinks() const {return links;}

        PathData &GetPathData() {return pathData;}

        const Pool <Condition> &GetConditions() const;

        bool HasAvailableColonySpots() const {return hasAvailableColonySpots;}

        bool HasUsedSimulationChange() const {return hasUsedSimulationChange;}

        void UseSimulationChange() {hasUsedSimulationChange = true;}
    };
}