#pragma once

#include "FlumenCore/Conventions.hpp"

namespace battle
{
    class BattleScene;

    struct BattleTile;
}

namespace battle::render
{
    class SacredFlameEffect
    {
        struct Particle
        {
            Position2 Position;

            float Size;

            float TintFactor;

            float Rotation;

            float Thickness;
        };

        bool isActive;

        float lifeTime;

        BattleTile *targetTile;

        float strength;

        container::Block <Particle, 512> particles;

    public:
        static constexpr auto DURATION = 2.0f;

        SacredFlameEffect(BattleTile *);

        bool Update();

        void TransferData();

        void Render();
    };
}