#pragma once

namespace battle
{
    class BattleScene;

    struct BattleTile;
}

namespace battle::render
{
    class FireballEffect
    {
        enum class Phases {FLIGHT, EXPLOSION};

        bool isActive;

        Phases phase;

        float lifeTime;

        BattleTile *startingTile;

        BattleTile *targetTile;

        int range;

        float strength;

    public:
        FireballEffect(BattleTile *, BattleTile *, int);

        bool Update();

        void Render();
    };
}