#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenBattle/CharacterActionData.h"

class Character;
struct BattleTile;
class BattleScene;
class Group;

class BattleController
{
    friend class BattleState;

    friend class ActionInfoPanel;

    friend class ArtificialController;

    BattleScene *battleScene;

    Group *playerControlledGroup;

    Group *computerControlledGroup;

    Character *selectedCharacter;

    Character *targetedCharacter;

    BattleTile *hoveredTile;

    CharacterActionData lastActionData;

    bool isPlayerInputEnabled;

    bool isInitiatingMove;

    bool isInitiatingTargeting;

    static BattleController *instance;

    void Initialize();

    void HandleSceneEnabled();

    void DetermineCharacterController();

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

    void EndTurn();

public:
    Delegate OnCharacterActed;

    Delegate OnCharacterSelected;

    Delegate OnActionSelected;

    Delegate OnSubactionSelected;

    void SelectCharacter(Character *);

    void TargetCharacter(Character *);

    BattleTile * GetHoveredTile() const {return hoveredTile;}

    Character * GetSelectedCharacter() const {return selectedCharacter;}

    Character * GetTargetedCharacter() const {return targetedCharacter;}

    const CharacterActionData & GetLastAction() const {return lastActionData;}

    bool IsMoveInitiated() const {return isInitiatingMove;}

    static BattleController * Get();
};