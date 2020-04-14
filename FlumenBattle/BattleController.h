#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenBattle/CharacterActionData.h"

class Character;
struct BattleTile;
class BattleScene;

class BattleController
{
    friend class BattleState;

    BattleScene * battleScene;

    Character * selectedCharacter;

    BattleTile * hoveredTile;

    CharacterActionData lastActionData;

    static BattleController * instance;

    void Initialize();

    void HandleSceneEnabled();

    void CheckCharacterMovement();

    void CheckTileSelection();

    void HandleSpacePressed();

    void HandleOnePressed();

    void HandleTwoPressed();

    void HandleThreePressed();

    void HandleFourPressed();

public:
    Delegate OnCharacterActed;

    Delegate OnCharacterSelected;

    Delegate OnActionSelected;

    Delegate OnSubactionSelected;

    void SelectCharacter(Character*);

    void TargetCharacter(Character*);

    BattleTile * GetHoveredTile() const {return hoveredTile;}

    Character * GetSelectedCharacter() const {return selectedCharacter;}

    const CharacterActionData & GetLastAction() const {return lastActionData;}

    static BattleController * Get();
};