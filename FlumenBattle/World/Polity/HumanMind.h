#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

#include "FlumenBattle/World/Polity/Mind.h"

enum class RaceTypes;

namespace world::settlement
{
    enum class ProductionOptions;
    enum class ProductionClasses;
    struct SettlementProductionType;
    class Settlement;
    class Shipment;
    class Building;
    struct SettlementTile;
    enum class TileImprovements;
    struct Resource;
    struct RaceGroup;
    class Cohort;
}

namespace world
{
    class WorldController;
    namespace tile {struct WorldTile;}
}

namespace world::science
{
    enum class Technologies;
}

namespace world::polity
{
    struct WorkInstruction;

    struct ProductionQueueSlot
    {
        settlement::ProductionOptions Option;

        int Priority;

        bool operator == (int priority) {return priority == Priority;}
    };

    class HumanMind : public Mind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        friend class world::WorldController;

        bool isWorkerPlaceModeActive {false};

        HumanMind();

        void EnableInput();

        void DisableInput();

        virtual void MakeDecision(Polity &) const override;

        virtual void UpdateWorkforce(Polity &) const override;

        virtual void DecideResearch(Polity &) const override;

        virtual void ProcessTrade(Polity &) const override;

        virtual void DecideBorders(Polity &) const override;

        void HandleWorkerPlacement();

        void HandleBorderExpansion();

        void HandleTileSettled();

        void HandleExplorationStarted();

        void HandleTileImproved();

        void HandleImproveSwipeLeft();

        void HandleImproveSwipeRight();

        void HandleSceneUpdateEnded();

        void UpdateSettlementWorkforce(settlement::Settlement *) const;

        void AddWorkInstruction(settlement::Settlement *, settlement::SettlementTile *);

        void AddWorkInstruction(settlement::Settlement *, settlement::Resource *);

        void RemoveWorkInstruction(settlement::Settlement *, settlement::SettlementTile *);

        void RemoveWorkInstruction(settlement::Settlement *, settlement::Resource *);

    public:
        Delegate OnProductionDecided;

        Delegate OnTradeShipment;

        Delegate OnTileWorkerChanged;

        Delegate OnPlayerSettlementPopIncrease;

        Delegate OnPlayerSettlementBorderExpanded;

        Delegate OnPlayerSettlementColonized;

        Delegate OnExplorationStarted;

        Delegate OnImprovementStarted;

        Delegate OnPlayerPolityDeleted;

        void ProcessProductionInput(settlement::ProductionOptions, settlement::ProductionClasses, settlement::Settlement *);

        void SetResearchTarget(science::Technologies);

        const settlement::Shipment &GetCurrentShipment();

        void RemoveWorkInstruction(WorkInstruction *);

        void RemoveWorkInstruction(settlement::Cohort *);

        void ChangeBuildingWorkforce(settlement::Building *, bool);

        void ChangeResourceWorkforce(settlement::Resource *, bool);

        const container::Pool <WorkInstruction> *GetSettlementInstructions() const;

        const container::Array <settlement::RaceGroup> &GetUnemployedByRace() const;

        const RaceTypes GetNextRaceToEmploy() const;

        void ShiftNextRaceToEmploy();

        void RegisterPopIncrease(settlement::Settlement *) const override;

        void RegisterTileExplored(settlement::Settlement *, tile::WorldTile *) const override;

        void RegisterProductionFinished(settlement::Settlement *, settlement::ProductionOptions) const override;

        void RegisterSettlementDeletion(settlement::Settlement *) const override;

        void RegisterMarkForDeletion() const override;

        const tile::WorldTile *GetSettleTarget(settlement::Settlement *) const;

        settlement::TileImprovements GetProposedImprovement();

        tile::WorldTile *GetLastExploredTile();

        settlement::Settlement *GetLastExplorerSettlement();

        void ProcessWorldUpdateData();

        void CancelProduction(ProductionQueueSlot *);

        const container::Pool <ProductionQueueSlot> &GetBuildingQueue() const;

        const container::Pool <ProductionQueueSlot> &GetRecruitmentQueue() const;
    };
}