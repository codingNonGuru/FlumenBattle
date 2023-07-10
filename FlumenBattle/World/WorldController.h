#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    namespace group
    {
        class Encounter;
    }
    
    class WorldTile;

    class WorldController : public Singleton<WorldController>
    {
        WorldTile *hoveredTile;

        bool isInEncounterMode {false};

        void HandleSceneUpdate();

        void CheckTileSelection();

        void HandleBattleStarted();

        void HandleSpacePressed();

        void HandleSpeedUpTime();

        void HandleSlowDownTime();

        void HandlePanUp();

        void HandlePanDown();

        void HandlePanLeft();

        void HandlePanRight();

        void HandleTravelPressed();

        void EnableHardInput();

        void DisableHardInput();

    public:
        void Enable();

        void Disable();

        WorldTile * GetHoveredTile() const {return hoveredTile;}

        group::Encounter * GetPlayerBattle() const;

        void HandlePlayerEncounterInitiated();

        void DisableEncounterMode();
    };
}