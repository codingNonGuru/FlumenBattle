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

    namespace polity
    {
        class Polity;
        class Faction;
    }

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
        friend class polity::Polity;

        friend class world::WorldScene;

        friend class AfflictionPerformer;

        friend class SettlementFactory;

        friend class SettlementAllocator;

        friend struct EventGenerator;

        friend class SettlementProductionFinisher;

        friend class SettlementLabel;

        friend class BuildingDamager;

        friend struct Resource;

        Word name;

        Color banner;

        world::WorldTile *location;

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer growth;

        Integer growthThreshold {1000};

        Integer cultureGrowth;

        bool areNearbyTilesTaken {false};

        Pool <SettlementTile> tiles;

        polity::Polity *polity;

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

        Integer GetIndustrialProduction() const;

        Integer GetScienceProduction() const;

        Integer GetPopulation() const {return population;}

        Integer GetGrowth() const {return growth;}

        float GetGrowthRatio() const {return float(growth) / float(growthThreshold);}

        Integer GetWorkedTiles() const;

        polity::Polity *GetPolity() const {return polity;}

        Settlement *GetRuler() const;

        int GetModifier(Modifiers) const;

        Resource *GetResource(ResourceTypes type) const {return resourceHandler.Get(type);}

        int GetStock(ResourceTypes type) const {return resourceHandler.Get(type)->Storage;}

        SettlementProduction *GetCurrentProduction() const {return currentProduction;}

        void SetPolity(polity::Polity *);

        void SetFaction(polity::Faction *newFaction) {faction = newFaction;}

        void KillPopulation();

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        void StrengthenPatrol();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();

        void UpdatePolitics();

        void PrepareTransport();

        void SendTransport();

        void ReceiveTransport();

        void AddPath(Path *path);

        Path *GetPathTo(Settlement *settlement);

        const Pool <Link> &GetLinks() {return links;}

        PathData &GetPathData() {return pathData;}
    };
}