#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::settlement
{
    enum class ProductionOptions;
    class Settlement;
    class Shipment;
}

namespace world
{
    class WorldController;
}

namespace world::science
{
    enum class Technologies;
}

namespace world::polity
{
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

        void HandleWorkerPlacement();

    public:
        Delegate OnProductionDecided;

        Delegate OnTradeShipment;

        void ProcessProductionInput(settlement::ProductionOptions, settlement::Settlement *);

        void SetResearchTarget(science::Technologies);

        const settlement::Shipment &GetCurrentShipment();
    };
}