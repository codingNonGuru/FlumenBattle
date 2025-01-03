#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::settlement
{
    enum class ProductionOptions;
    class Settlement;
    class Shipment;
    class Building;
    struct SettlementTile;
    enum class TileImprovements;
}

namespace world
{
    class WorldController;
    class WorldTile;
}

namespace world::science
{
    enum class Technologies;
}

namespace world::polity
{
    struct WorkInstruction;

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

        void HireWorker(settlement::Settlement *, settlement::SettlementTile *);

        void HireWorker(settlement::Settlement *, settlement::Building *);

        void FireWorker(settlement::Settlement *, settlement::SettlementTile *);

        void FireWorker(settlement::Settlement *, settlement::Building *);

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

        void ProcessProductionInput(settlement::ProductionOptions, settlement::Settlement *);

        void SetResearchTarget(science::Technologies);

        const settlement::Shipment &GetCurrentShipment();

        void ChangeBuildingWorkforce(settlement::Building *, bool);

        const container::Pool <WorkInstruction> *GetSettlementInstructions() const;

        void RegisterPopIncrease(settlement::Settlement *) const override;

        void RegisterTileExplored(settlement::Settlement *, WorldTile *) const override;

        void RegisterMarkForDeletion() const override;

        const WorldTile *GetSettleTarget(settlement::Settlement *) const;

        settlement::TileImprovements GetProposedImprovement();

        WorldTile *GetLastExploredTile();

        settlement::Settlement *GetLastExplorerSettlement();

        void ProcessWorldUpdateData();
    };
}