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
    namespace tile {struct WorldTile;}
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
        tile::WorldTile *Tile;

        int Progress;

        float GetProgressFactor() const;

        bool operator == (const tile::WorldTile *tile) const {return Tile == tile;}
    };

    struct ExploreResult
    {
        tile::WorldTile *Tile;

        bool operator == (const tile::WorldTile *tile) const {return Tile == tile;}
    };

    struct ImprovementTarget
    {
        SettlementTile *Tile;

        TileImprovements ImprovementType;
    };

    struct SimulationUpdateData
    {
        Settlement *Settlement;
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

        bool isValid;

        int uniqueId;

        int foundationDate;

        Word name;

        Color banner;

        tile::WorldTile *location;

        world::SimulationDomain *simulationDomain;

        SimulationLevels simulationLevel;

        int timeSinceSimulationChange;

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

        SettlementProduction *buildingProduction;

        SettlementProduction *groupProduction;

        ModifierManager modifierManager;

        ConditionManager *conditionManager;
        
        BuildingManager *buildingManager;

        ResourceHandler resourceHandler;

        PopHandler popHandler;

        int storage;

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

        int turnsUntilDeletion;

        void Initialize(Word, Color, tile::WorldTile *);

        void GrowBorders();

    public:
        bool IsValid() const {return isValid;}

        bool ShouldBeDeleted() const {return isValid == false && turnsUntilDeletion == 0;}

        bool IsDeepSettlement() const;

        int GetFoundationDate() const {return foundationDate;}

        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        tile::WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        int GetStorage() const {return storage;}

        Integer GetIndustrialProduction() const;

        Integer GetRecruitmentCapacity() const;

        Integer GetScienceProduction() const;

        bool CanGrowBorders() const;

        bool CanExpandHere(tile::WorldTile *) const;

        bool CanSettleHere(tile::WorldTile *) const;

        int GetExpansionCost(tile::WorldTile *) const;

        bool CanAffordToExpandHere(tile::WorldTile *) const;

        float GetCultureProgress() const;

        Integer GetPopulation() const;

        void IncreasePopulation();

        PopHandler &GetPopulationHandler() {return popHandler;}

        const ResourceHandler &GetResourceHandler() const {return resourceHandler;}

        const container::Pool <Cohort> &GetPopCohorts() const;

        int GetNeedSatisfaction(ResourceTypes resource) const {return popHandler.GetNeedSatisfaction(resource);}

        bool IsContent() const {return popHandler.IsContent();}

        bool IsHappy() const {return popHandler.IsHappy();}

        bool IsEcstatic() const {return popHandler.IsEcstatic();}

        float GetHappinessRatio() const {return popHandler.GetHappinessRatio();}

        Integer GetGrowth() const {return popHandler.GetGrowth();}

        float GetGrowthRatio() const {return popHandler.GetGrowthRatio();}

        AbundanceLevels GetHousingAdequacy() const;

        Integer GetWorkedTiles() const;

        bool IsTileImproved(tile::WorldTile *) const;

        Integer GetFreeWorkerCount() const;

        Integer GetWorkerCount(ResourceTypes) const;

        const group::GroupDynamics &GetGroupDynamics() const {return *groupDynamics;}

        polity::Polity *GetPolity() const {return polity;}

        Settlement *GetRuler() const;

        int GetModifier(Modifiers) const;

        Resource *GetResource(ResourceTypes type) const {return resourceHandler.Get(type);}

        int GetResourcePrice(ResourceTypes type) const {return resourceHandler.Get(type)->Type->Value;}

        int GetMulePrice() const;

        int GetStock(ResourceTypes type) const {return resourceHandler.Get(type)->Storage;}

        SettlementProduction *GetBuildingProduction() const {return buildingProduction;}

        SettlementProduction *GetGroupProduction() const {return groupProduction;}

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

        void Deepen();

        int Loot(bool, int);

        void Pillage(int);

        void SetupSimulation();

        tile::WorldTile *FindColonySpot();

        void RemoveGroup(const group::GroupCore &);

        void SetPolity(polity::Polity *);

        void SetFaction(polity::Faction *newFaction) {faction = newFaction;}

        void KillPopulation();

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        bool HireWorker(Building *);

        bool HireWorker(ResourceTypes);

        void FireWorker(ResourceTypes);

        void FireWorker(Job *);

        void FireAllWorkers();

        void StrengthenPatrol();

        void StrengthenGarrison();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();

        void EnforceSimulationLevel();

        void DecideProduction(ProductionClasses);

        void SetBuildingProduction(ProductionOptions);

        void SetGroupProduction(ProductionOptions);

        void UpdatePolitics();

        void UpdateDistanceToCapital();

        void PrepareTransport();

        void SendTransport();

        void ReceiveTransport(ResourceTypes, int);

        void FinishTradeUpdate();

        float GetTradeProgress() const {return tradeHandler.GetProgress(*this);}

        const TradeHandler &GetTradeHandler() const {return tradeHandler;}

        void AddPath(Path *);

        void RemoveLink(Settlement *);

        void UpdateColonialMap();

        Path *GetPathTo(Settlement *settlement);

        const Pool <Link> &GetLinks() const {return links;}

        PathData &GetPathData() {return pathData;}

        const Pool <Condition> &GetConditions() const;

        bool HasAvailableColonySpots() const {return hasAvailableColonySpots;}

        bool IsPlayerControlled() const;

        const Race *GetRace() const;

        const Shipment &GetLastOutgoingShipment() {return tradeHandler.lastOutgoingShipment;}

        void AddSettlers();

        void RemoveSettlers();

        bool HasMaximumSettlers() const;

        bool HasAnySettlers() const;

        bool IsExploring(tile::WorldTile *) const;

        bool IsExploring() const;

        const Exploration &GetCurrentExploration() {return currentExploration;}

        tile::WorldTile *GetCurrentlyExploredTile() {return currentExploration.Tile;}

        bool HasExplored(tile::WorldTile *) const;

        container::Array <tile::WorldTile *> &GetExploredTiles() const;

        const container::Pool <ExploreResult> &GetFinishedExplorations() const {return finishedExplorations;}

        bool CanExploreHere(tile::WorldTile *) const;

        void StartExploring(tile::WorldTile *);

        void StopExploring();

        void RemoveFinishedExploration(tile::WorldTile *);

        void AddExplorationProgress(int);

        const ExplorationReward &GetLastExplorationReward() const;

        bool CanImproveHere(tile::WorldTile *, TileImprovements) const;

        void StartImprovingTile(tile::WorldTile *, TileImprovements);

        bool IsImprovingAnyTile() const {return currentImprovement.Tile != nullptr;}

        bool IsImprovingTile(SettlementTile *, TileImprovements) const;

        bool IsImprovingTile(tile::WorldTile *, TileImprovements) const;

        const ImprovementTarget &GetCurrentImprovement() const {return currentImprovement;}

        void CancelImproving();

        bool HasImprovement(SettlementTile *, TileImprovements) const;

        bool HasImprovement(tile::WorldTile *, TileImprovements) const;

        bool IsAbandoned() const;

        bool IsRuins() const;

        bool IsCompletelyGone() const;

        int GetAbandonmentSeverity() const;

        int GetHighestPopulationEver() const;

        void PromptWorkReorganizing() {needsToReorganizeWork = true;}

        void MarkForDeletion();
    };
}