#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

namespace battle
{
    struct BattleTile;

    class BattleAnimator : public core::Singleton <BattleAnimator>
    {
        friend class core::Singleton <BattleAnimator>;

        bool isAnimating {false};

        float time {0.0f};

        Event OnFinished;

        BattleAnimator();

        void Update();

        BattleTile *GetNextTile();

        void Advance();

    public:
        void FollowPathMovement(Event);

        bool IsWorking() const {return isAnimating;}
    };
}