#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::polity
{
    class MachineMind : public Mind, public core::Singleton <MachineMind>
    {
        virtual void MakeDecision(Polity &) const override;

        virtual void UpdateWorkforce(Polity &) const override;

        virtual void DecideResearch(Polity &) const override;

        virtual void ProcessTrade(Polity &) const override {}

        virtual void DecideBorders(Polity &) const override;

        void RegisterPopIncrease(settlement::Settlement *) const override {}

        void GrowBorders(settlement::Settlement *);
    };
}