#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

namespace battle
{
    class BattleAnimator : public core::Singleton <BattleAnimator>
    {
        friend class core::Singleton <BattleAnimator>;

        bool isAnimating {false};

        float time {0.0f};

        Event OnFinished;

        BattleAnimator();

        void Update();

    public:
        void FollowPathMovement(Event);
    };
}