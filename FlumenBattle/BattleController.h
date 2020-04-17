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

    friend class HumanController;

    BattleScene *battleScene;

    Group *playerControlledGroup;

    Group *computerControlledGroup;

    BattleTile *targetedTile;

    Character *selectedCharacter;

    Character *targetedCharacter;

    CharacterActionData lastActionData;

    bool isPlayerInputEnabled;

    static BattleController *instance;

    void Initialize();

    void HandleSceneEnabled();

    void DetermineCharacterController();

    void Move();

    void CheckTileSelection();

    void SelectAction(Integer);

    void SelectSubaction(Integer);

    void Act();

    //void ChangeActionSelection(Integer);

    void EndTurn();

public:
    Delegate OnCharacterActed;

    Delegate OnCharacterSelected;

    Delegate OnActionSelected;

    Delegate OnSubactionSelected;

    void SelectCharacter(Character *);

    void TargetCharacter(Character *);

    void TargetTile(BattleTile * tile) {targetedTile = tile;}

    BattleTile * GetTargetedTile() const {return targetedTile;}

    Character * GetSelectedCharacter() const {return selectedCharacter;}

    Character * GetTargetedCharacter() const {return targetedCharacter;}

    const CharacterActionData & GetLastAction() const {return lastActionData;}

    static BattleController * Get();
};