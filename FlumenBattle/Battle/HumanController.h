#pragma once

#include "FlumenCore/Conventions.hpp"

class Delegate;

namespace utility
{
    template <typename TileType>
    struct PathData;
}

namespace battle
{
    struct BattleTile;
    class Combatant;

    class HumanController
    {
        friend class BattleController;

    public:
        static constexpr auto MAXIMUM_PATH_LENGTH = 15;

    private:
        bool isInitiatingMove;

        bool isInitiatingTargeting;

        BattleTile *hoveredTile;

        void Initialize();

        void EnablePlayerInput();

        void DisablePlayerInput();

        void CheckCharacterMovement();

        void CheckTileSelection();

        void HandleSpacePressed();

        template <int>
        void HandleNumberPressed();

        template <int>
        void HandleCtrlNumberPressed();

        void HandleOnePressed();

        void HandleTwoPressed();

        void HandleThreePressed();

        void HandleFourPressed();

        void ChangeActionSelection(Integer, bool);

        void HandleMPressed();

        void HandleTPressed();

        void HandleAPressed();

        void HandleSceneUpdate();

        HumanController();

    public:
        Delegate *OnTargetInitiated;

        Delegate *OnTargetAbandoned;

        bool IsMoveInitiated() const {return isInitiatingMove;}

        bool IsInitiatingTargeting() const {return isInitiatingTargeting;}

        void TargetCombatant(Combatant *);

        void TargetTile(BattleTile *);

        BattleTile * GetHoveredTile() const {return hoveredTile;}

        const utility::PathData <BattleTile> &GetHoveredPath();

        Position2 GetHoveredPosition() const;

        static HumanController * Get()
        {
            static HumanController controller;

            return &controller;
        }
    };
}