#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class Battle;

    class WorldController : public Singleton<WorldController>
    {
        Battle *playerBattle;

        void HandleSceneUpdate();

        void StartBattle();

        void HandleSpacePressed();

        void HandleSpeedUpTime();

        void HandleSlowDownTime();

        void HandlePanUp();

        void HandlePanDown();

        void HandlePanLeft();

        void HandlePanRight();

    public:
        void Enable();

        void Disable();

        Battle * GetPlayerBattle() const {return playerBattle;}
    };
}