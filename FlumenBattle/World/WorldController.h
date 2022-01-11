#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class Battle;
    class WorldTile;

    class WorldController : public Singleton<WorldController>
    {
        Battle *playerBattle;

        WorldTile *hoveredTile;

        void HandleSceneUpdate();

        void CheckTileSelection();

        void StartBattle();

        void HandleSpacePressed();

        void HandleSpeedUpTime();

        void HandleSlowDownTime();

        void HandlePanUp();

        void HandlePanDown();

        void HandlePanLeft();

        void HandlePanRight();

        void HandleTravelPressed();

    public:
        void Enable();

        void Disable();

        WorldTile * GetHoveredTile() const {return hoveredTile;}

        Battle * GetPlayerBattle() const {return playerBattle;}
    };
}