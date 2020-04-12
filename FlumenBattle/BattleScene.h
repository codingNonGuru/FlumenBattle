#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenBattle/CharacterActionData.h"

class CharacterModel;
class Group;
class CharacterInfo;
class Character;
class BattleTileModel;
class BattleMap;
class BattleTile;

struct Turn
{
    Character* Character;

    Integer Initiative;

    bool operator > (const Turn &other)
    {
        return Initiative > other.Initiative;
    }
};

class BattleScene : public Scene 
{
    friend class CharacterModel;

    friend class BattleTileModel;

    BattleMap * battleMap;

    CharacterModel * characterModel;

    BattleTileModel * battleTileModel;

    Character * selectedCharacter;

    BattleTile * hoveredTile;

    Array <Group> groups;

    Array <Turn> turnOrder;

    Turn * turn;

    CharacterActionData lastActionData;

    void Initialize() override;

    void Render() override;

    void Update() override;

    void CheckTileSelection();

    void DetermineTurnOrder();

    void CheckCharacterMovement();

    void HandleSpacePressed();

    void HandleOnePressed();

    void HandleTwoPressed();

public:
    Delegate OnCharacterActed;

    Delegate OnCharacterSelected;

    Delegate OnActionSelected;

    Delegate OnSubactionSelected;

    BattleScene();

    void SelectCharacter(Character*);

    void TargetCharacter(Character*);

    Character* GetSelectedCharacter() const {return selectedCharacter;}

    BattleMap* GetBattleMap() {return battleMap;}

    Turn* GetTurn() {return turn;}

    bool IsCharactersTurn(Character*) const;

    const CharacterActionData & GetLastAction() const {return lastActionData;}
};