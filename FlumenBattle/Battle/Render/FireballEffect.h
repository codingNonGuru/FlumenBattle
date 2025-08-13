#pragma once

#include "FlumenCore/Conventions.hpp"

namespace battle
{
    class BattleScene;

    struct BattleTile;
}

namespace battle::render
{
    class FireballEffect
    {
        struct Particle
        {
            Position2 Position;

            float Size;

            float TintFactor;
        };

        enum class Phases {FLIGHT, EXPLOSION};

        bool isActive;

        Phases phase;

        float lifeTime;

        BattleTile *startingTile;

        BattleTile *targetTile;

        int range;

        float strength;

        container::Block <Particle, 10> flightParticles;

        container::Block <Particle, 19> boomParticles;

    public:
        static const auto DURATION = 2.0f;

        FireballEffect(BattleTile *, BattleTile *, int);

        bool Update();

        void Render();
    };
}