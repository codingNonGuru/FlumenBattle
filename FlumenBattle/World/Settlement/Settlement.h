#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Settlement/Shipment.h"
#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/World/Settlement/TradeHandler.h"

struct Race;

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
        class MachineMind;
        class HumanMind;
    }

    namespace group
    {
        class GroupDynamics;
        class GroupCore;
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
    struct SettlementTile;
    struct ExplorationReward;

    struct Link
    {
        Path *Path;

        Settlement *Other;

        int Traffic {0};

        bool operator== (const settlement::Path &path) const;

        bool operator== (const settlement::Settlement *other) {return Other == other;}
    };

    struct Exploration
    {
        WorldTile *Tile;

        int Progress;

        float GetProgressFactor() const;

        bool operator == (const WorldTile *tile) const {return Tile == tile;}
    };

    struct ExploreResult
    {
        WorldTile *Tile;

        bool operator == (const WorldTile *tile) const {return Tile == tile;}
    };

    struct ImprovementTarget
    {
        SettlementTile *Tile;

        TileImprovements ImprovementType;
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

        friend class polity::MachineMind;

        friend class polity::HumanMind;

        Word name;

        Color banner;

        world::WorldTile *location;

        const Race *race;

        world::SimulationDomain *simulationDomain;

        SimulationLevels simulationLevel;

        int timeSinceSimulationChange;

        bool hasUsedSimulationChange {true};

        group::GroupDynamics *groupDynamics;

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

        PopHandler popHandler;

        int storage {500};

        //Shipment lastOutgoingShipment;
        TradeHandler tradeHandler;

        Settlement *tradeDestination;

        Integer lastShipmentTime;

        Integer independenceDrive {0};

        polity::Faction *faction {nullptr};

        struct PathData
        {
            bool IsVisited = false;

            bool IsToBeVisited = false;
        } pathData;

        container::Pool <ExploreResult> finishedExplorations;

        Exploration currentExploration;

        ImprovementTarget currentImprovement;

        bool isValid;

        int turnsUntilDeletion;

        void Initialize(Word, Color, world::WorldTile *, const Race *);

        void GrowBorders();

    public:
        bool IsValid() const {return isValid;}

        bool ShouldBeDeleted() const {return isValid == false && turnsUntilDeletion == 0;}

        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        world::WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        Integer GetIndustrialProduction() const;

        Integer GetScienceProduction() const;

        bool CanGrowBorders() const;

        bool CanExpandHere(WorldTile *) const;

        bool CanSettleHere(WorldTile *) const;

        int GetExpansionCost(WorldTile *) const;

        bool CanAffordToExpandHere(WorldTile *) const;

        float GetCultureProgress() const;

        Integer GetPopulation() const;

        void IncreasePopulation();

        PopHandler &GetPopulationHandler() {return popHandler;}

        int GetNeedSatisfaction(ResourceTypes resource) const {return popHandler.GetNeedSatisfaction(resource);}

        bool IsContent() const {return popHandler.IsContent();}

        bool IsHappy() const {return popHandler.IsHappy();}

        bool IsEcstatic() const {return popHandler.IsEcstatic();}

        float GetHappinessRatio() const {return popHandler.GetHappinessRatio();}

        Integer GetGrowth() const {return popHandler.GetGrowth();}

        float GetGrowthRatio() const {return popHandler.GetGrowthRatio();}

        AbundanceLevels GetHousingAdequacy() const;

        Integer GetWorkedTiles() const;

        bool IsTileImproved(WorldTile *) const;

        Integer GetFreeWorkerCount() const;

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

        int GetTotalBuildingCount() const;

        const container::Array <Building *> &GetBuildingsThatProduce(ResourceTypes) const;

        const Building &GetBuilding(BuildingTypes) const;

        polity::Faction *GetFaction() const {return faction;}

        int GetDistanceToCapital() const {return distanceToCapital;}

        world::SimulationLevels GetSimulationLevel() const;

        bool IsDefended() const;

        int GetDefenceSneakDC() const;

        int GetBribeGarrisonDC() const;

        int GetWallsLevel() const;

        int GetStandingBuildingCount() const;

        bool IsLootable() const;

        bool IsPillageable() const;

        int GetLootDC() const;

        int GetPillageDC() const;

        int Loot(bool, int);

        void Pillage(int);

        void SetupSimulation();

        world::WorldTile *FindColonySpot();

        void RemoveGroup(const group::GroupCore &);

        void SetPolity(polity::Polity *);

        void SetFaction(polity::Faction *newFaction) {faction = newFaction;}

        void KillPopulation();

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        bool HireWorker(Building *);

        void FireAllWorkers();

        void StrengthenPatrol();

        void StrengthenGarrison();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();

        void DecideProduction();

        void SetProduction(ProductionOptions);

        void UpdatePolitics();

        void UpdateDistanceToCapital();

        void PrepareTransport();

        void SendTransport();

        void ReceiveTransport(ResourceTypes, int);

        void FinishTradeUpdate();

        float GetTradeProgress() const {return tradeHandler.GetProgress(*this);}

        void AddPath(Path *);

        void RemoveLink(Settlement *);

        void UpdateColonialMap();

        Path *GetPathTo(Settlement *settlement);

        const Pool <Link> &GetLinks() const {return links;}

        PathData &GetPathData() {return pathData;}

        const Pool <Condition> &GetConditions() const;

        bool HasAvailableColonySpots() const {return hasAvailableColonySpots;}

        bool HasUsedSimulationChange() const {return hasUsedSimulationChange;}

        void UseSimulationChange() {hasUsedSimulationChange = true;}

        bool IsPlayerControlled() const;

        const Race *GetRace() const {return race;}

        const Shipment &GetLastOutgoingShipment() {return tradeHandler.lastOutgoingShipment;}

        void AddSettlers();

        void RemoveSettlers();

        bool HasMaximumSettlers() const;

        bool HasAnySettlers() const;

        bool IsExploring(WorldTile *) const;

        bool IsExploring() const;

        const Exploration &GetCurrentExploration() {return currentExploration;}

        WorldTile *GetCurrentlyExploredTile() {return currentExploration.Tile;}

        bool HasExplored(WorldTile *) const;

        container::Array <WorldTile *> &GetExploredTiles() const;

        const container::Pool <ExploreResult> &GetFinishedExplorations() const {return finishedExplorations;}

        bool CanExploreHere(WorldTile *) const;

        void StartExploring(WorldTile *);

        void StopExploring();

        void RemoveFinishedExploration(WorldTile *);

        void AddExplorationProgress(int);

        const ExplorationReward &GetLastExplorationReward() const;

        bool CanImproveHere(WorldTile *, TileImprovements) const;

        void StartImprovingTile(WorldTile *, TileImprovements);

        bool IsImprovingAnyTile() const {return currentImprovement.Tile != nullptr;}

        bool IsImprovingTile(SettlementTile *, TileImprovements) const;

        bool IsImprovingTile(WorldTile *, TileImprovements) const;

        const ImprovementTarget &GetCurrentImprovement() const {return currentImprovement;}

        void CancelImproving();

        bool HasImprovement(SettlementTile *, TileImprovements) const;

        bool HasImprovement(WorldTile *, TileImprovements) const;

        bool IsAbandoned() const;

        bool IsRuins() const;

        bool IsCompletelyGone() const;

        int GetAbandonmentSeverity() const;

        void PromptWorkReorganizing() {needsToReorganizeWork = true;}

        void MarkForDeletion();
    };
}