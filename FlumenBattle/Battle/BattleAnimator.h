#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Types.hpp"

namespace battle
{
    namespace render
    {
        struct FireballEffect;
    }

    struct BattleTile;

    class BattleAnimator : public core::Singleton <BattleAnimator>
    {
        friend class core::Singleton <BattleAnimator>;

        bool isAnimating {false};

        float time {0.0f};

        float jumpLength;

        float totalLength;

        Event OnFinished;

        BattleAnimator();

        void Update();

        BattleTile *GetNextTile();

        void Advance();

    public:
        void FollowPathMovement(Event);

        void SetupActionAnimation(Event);

        bool IsWorking() const {return isAnimating;}

        float GetAnimationLength(world::character::CharacterActions, SpellTypes = SpellTypes::NONE) const;

        const render::FireballEffect &GetFireballEffect() const;
    };
}