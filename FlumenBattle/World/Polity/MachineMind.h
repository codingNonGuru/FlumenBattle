#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Polity/Mind.h"

namespace world::polity
{
    class MachineMind : public Mind, public core::Singleton <MachineMind>
    {
        virtual void MakeDecision(Polity &) const override;

        virtual void UpdateWorkforce(Polity &) const override;
    };
}