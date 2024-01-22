#pragma once

#include "FlumenCore/Singleton.h"

namespace battle::sound
{
    class BattleMixer : public core::Singleton <BattleMixer>
    {
        void HandleTargetingInitiated();

        void HandleCombatantActed();

    public:
        void Initialize();
    };
}