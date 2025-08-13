#pragma once

#include "FlumenCore/Conventions.hpp"

namespace battle
{
    class BattleScene;

    struct BattleTile;

    class Combatant;
}

namespace battle::render
{
    class BurningHandsEffect
    {
        struct Particle
        {
            Position2 Position;

            float Size;

            float TintFactor;

            float Opacity;
        };

        bool isActive;

        float lifeTime;

        float strength;

        container::SmartBlock <Particle, 512> particles;

    public:
        static const auto DURATION = 2.0f;

        BurningHandsEffect(Combatant &, BattleTile *);

        bool Update();

        void Render();
    };
}