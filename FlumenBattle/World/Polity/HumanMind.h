#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::polity
{
    class HumanMind : public Mind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        HumanMind();

        virtual void MakeDecision(Polity &) const override;
    };
}