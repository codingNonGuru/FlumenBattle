#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::settlement
{
    enum class ProductionOptions;
    class Settlement;
}

namespace world::polity
{
    class HumanMind : public Mind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        HumanMind();

        virtual void MakeDecision(Polity &) const override;

    public:
        void ProcessProductionInput(settlement::ProductionOptions, settlement::Settlement *);
    };
}