#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Observer.h"

namespace utility
{
    template <typename TileType>
    struct PathData;
}

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

        bool isGrabbingScreen {false};

        Position2 grabStartPosition;

        Position2 grabPositionDelta;

        bool isInEncounterMode {false};

        bool isResourceDisplayActive {false};

        bool isFoodDisplayActive {false};

        bool isTravelPlanActive {false};

        bool canColonize {true};

        struct CharacterSelection
        {
            bool IsSelected;

            Integer Index;
        } characterSelection {false, 0};

        Delegate onInventoryPressed;

        Delegate onCharacterSelected;

        WorldController();

        void CheckTileSelection();

        void HandleBattleStarted();

        void HandleSpacePressed();

        void HandleSpeedUpTime();

        void HandleSlowDownTime();

        void HandlePanUp();

        void HandlePanDown();

        void HandlePanLeft();

        void HandlePanRight();

        void HandleGrabPressed();

        void HandleGrabReleased();

        void HandleTravelPressed();

        void HandleResourceDisplayPressed();

        void HandleFoodDisplayPressed();

        void HandleCharacterSelected();

        void HandleInventoryPressed();

        void HandleColonizationSwitch();

        void HandleTravelModeToggle();

        void EnableHardInput();

        void DisableHardInput();

        CharacterSelection GetSelectionData() const {return characterSelection;}

    public:
        void Initialize();

        void Update();

        void Enable();

        void Disable();

        WorldTile * GetHoveredTile() const {return hoveredTile;}

        group::Encounter * GetPlayerBattle() const;

        void HandlePlayerEncounterInitiated();

        void DisableEncounterMode();

        bool CanBuyFood();

        bool CanBuyMule();

        void BuyFood();

        bool ShouldDisplayResources() const {return isResourceDisplayActive;}

        bool ShouldDisplayNearbyFood() const {return isFoodDisplayActive;}

        bool CanColonize() const {return canColonize;}

        bool IsTravelPlanActive() const {return isTravelPlanActive;}

        utility::PathData <WorldTile> GetPlannedPath() const;
    };
}