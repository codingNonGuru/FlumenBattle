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
    
    namespace tile {struct WorldTile;}
    class WorldInterface;

    class WorldController : public core::Singleton <WorldController>
    {
        friend class core::Singleton <WorldController>;

        friend class WorldInterface;

        tile::WorldTile *hoveredTile;

        bool isGrabbingScreen {false};

        Position2 grabStartPosition;

        Position2 grabPositionDelta;

        bool isInEncounterMode {false};

        bool isResourceDisplayActive {false};

        bool shouldResourceDisplayIncludeBonus {false};

        bool isMetalDisplayActive {false};

        bool isFoodDisplayActive {false};

        bool isTimberDisplayActive {false};

        bool isFiberDisplayActive {false};

        bool isImprovementDisplayActive {false};

        bool isWorkerPlaceModeActive {false};

        bool isTravelPlanActive {false};

        bool isBorderExpandActive {false};

        bool isSettleModeActive {false};

        bool isExploreModeActive {false};

        bool isTileDevelopModeActive {false};

        bool canColonize {true};

        struct CharacterSelection
        {
            bool IsSelected;

            Integer Index;
        } characterSelection {false, 0};

        Delegate onInventoryPressed;

        Delegate onCharacterSelected;

        Delegate onWorkerPlaceModeEntered;

        Delegate onRuleMenuPressed;

        int reliefDisplayMode;

        WorldController();

        void CheckTileSelection();

        void HandleWorldCanvasHovered();

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

        void HandleCenterPlayerPressed();

        void HandleTravelPressed();

        void HandleTravelReleased();

        void HandleResourceDisplayPressed();

        void HandleMetalDisplayPressed();

        void HandleFoodDisplayPressed();

        void HandleTimberDisplayPressed();

        void HandleFiberDisplayPressed();

        void HandleImprovementDisplayPressed();

        void HandleWorkerPlacePressed();

        void HandleCharacterSelected();

        void HandleInventoryPressed();

        void HandleColonizationSwitch();

        void HandleTravelModeToggle();

        void HandleSettlementExited();

        void HandleRuleMenuPressed();
        
        void HandleBorderExpandPressed();

        void HandleSettleModePressed();

        void HandleExploreModePressed();

        void HandleTileDevelopModePressed();

        void HandleReliefDisplayPressed();

        void EnableHardInput();

        void DisableHardInput();

        CharacterSelection GetSelectionData() const {return characterSelection;}

    public:
        Delegate OnResourceDisplayPressed;

        Delegate OnWorkerPlaceModeToggled;

        Delegate OnExploreModeToggled;

        Delegate OnTileDevelopModeEnabled;

        Delegate OnTileDevelopModeToggled;

        void Initialize();

        void Update();

        void Enable();

        void Disable();

        tile::WorldTile * GetHoveredTile() const {return hoveredTile;}

        group::Encounter * GetPlayerBattle() const;

        void HandlePlayerEncounterInitiated();

        void DisableEncounterMode();

        bool CanAttackGarrison();

        bool CanConquerSettlement();

        bool CanLootSettlement();

        bool CanPillageSettlement();

        bool CanBuyFood();

        bool CanBuyMule();

        void AttackGarrison();

        void LootSettlement();

        void PillageSettlement();

        void BuyFood();

        bool ShouldDisplayResources() const {return isResourceDisplayActive;}

        bool ShouldResourceDisplayIncludeBonus() const {return shouldResourceDisplayIncludeBonus;}

        bool ShouldDisplayMetal() const {return isMetalDisplayActive;}

        bool ShouldDisplayNearbyFood() const {return isFoodDisplayActive;}

        bool ShouldDisplayNearbyTimber() const {return isTimberDisplayActive;}

        bool ShouldDisplayNearbyFiber() const {return isFiberDisplayActive;}

        bool ShouldDisplayImprovements() const {return isImprovementDisplayActive;}

        bool IsWorkerPlaceModeActive() const {return isWorkerPlaceModeActive;}

        bool CanColonize() const {return canColonize;}

        bool IsTravelPlanActive() const {return isTravelPlanActive;}

        bool IsBorderExpandActive() const {return isBorderExpandActive;}

        bool IsSettleModeActive() const {return isSettleModeActive;}

        bool IsExploreModeActive() const {return isExploreModeActive;}

        bool IsTileDevelopModeActive() const {return isTileDevelopModeActive;}

        utility::PathData <tile::WorldTile> GetPlannedPath() const;

        void CancelActiveModes();

        int GetDisplayMode() const {return reliefDisplayMode;}
    };
}