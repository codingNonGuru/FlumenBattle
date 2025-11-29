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
    class FrostRayEffect
    {
        bool isActive;

        float lifeTime;

        BattleTile *startingTile;

        BattleTile *targetTile;

        float strength;

    public:
        static constexpr auto DURATION = 1.0f;

        FrostRayEffect(BattleTile *, BattleTile *);

        bool Update();

        void Render();
    };
}