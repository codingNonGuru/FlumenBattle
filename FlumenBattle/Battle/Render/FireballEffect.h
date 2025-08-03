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
        bool isActive;

        float lifeTime;

        BattleTile *tile;

        int range;

        float strength;

    public:
        FireballEffect(BattleTile *, int);

        bool Update();

        void Render();
    };
}