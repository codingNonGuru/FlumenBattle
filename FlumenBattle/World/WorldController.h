#pragma once

#include "FlumenCore/Singleton.h"

class Delegate;

namespace world
{
    namespace group
    {
        class Encounter;
    }
    
    class WorldTile;
    class WorldInterface;

    class WorldController : public core::Singleton <WorldController>
    {
        friend class core::Singleton <WorldController>;

        friend class WorldInterface;

        WorldTile *hoveredTile;

        bool isInEncounterMode {false};

        bool isResourceDisplayActive {false};

        bool canColonize {true};

        struct CharacterSelection
        {
            bool IsSelected;

            Integer Index;
        } characterSelection {false, 0};

        Delegate *onInventoryPressed;

        Delegate *onCharacterSelected;

        WorldController();

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

        void HandleResourceDisplayPressed();

        void HandleCharacterSelected();

        void HandleInventoryPressed();

        void HandleColonizationSwitch();

        void EnableHardInput();

        void DisableHardInput();

        CharacterSelection GetSelectionData() const {return characterSelection;}

    public:
        void Enable();

        void Disable();

        WorldTile * GetHoveredTile() const {return hoveredTile;}

        group::Encounter * GetPlayerBattle() const;

        void HandlePlayerEncounterInitiated();

        void DisableEncounterMode();

        bool ShouldDisplayResources() const {return isResourceDisplayActive;}

        bool CanColonize() const {return canColonize;}
    };
}