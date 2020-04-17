#pragma once

#include "FlumenCore/Conventions.hpp"

struct BattleTile;
class Character;

class HumanController
{
    friend class BattleController;
    
    friend class ActionInfoPanel;

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

public:
    bool IsMoveInitiated() const {return isInitiatingMove;}

    void TargetCharacter(Character *);

    BattleTile * GetHoveredTile() const {return hoveredTile;}

    static HumanController * Get()
    {
        static HumanController controller;

        return &controller;
    }
};