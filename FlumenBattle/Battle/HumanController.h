#pragma once

#include "FlumenCore/Conventions.hpp"

class Delegate;

namespace battle
{
    struct BattleTile;
    class Combatant;

    class HumanController
    {
        friend class BattleController;

        //void DetermineActionCourse();

        //void MoveCharacter();

        //void UpdateCharacter();

        bool isInitiatingMove;

        bool isInitiatingTargeting;

        BattleTile *hoveredTile;

        void Initialize();

        void EnablePlayerInput();

        void DisablePlayerInput();

        void CheckCharacterMovement();

        void CheckTileSelection();

        void HandleSpacePressed();

        void HandleOnePressed();

        void HandleTwoPressed();

        void HandleThreePressed();

        void HandleFourPressed();

        void ChangeActionSelection(Integer);

        void HandleMPressed();

        void HandleTPressed();

        void HandleAPressed();

        HumanController();

    public:
        Delegate *OnTargetInitiated;

        Delegate *OnTargetAbandoned;

        bool IsMoveInitiated() const {return isInitiatingMove;}

        bool IsInitiatingTargeting() const {return isInitiatingTargeting;}

        void TargetCombatant(Combatant *);

        BattleTile * GetHoveredTile() const {return hoveredTile;}

        static HumanController * Get()
        {
            static HumanController controller;

            return &controller;
        }
    };
}