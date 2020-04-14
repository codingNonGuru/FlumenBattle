#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenBattle/CharacterActionData.h"

class BattleInterface;
class Group;
class CharacterInfo;
class Character;
class BattleTileModel;
class BattleMap;
class BattleTile;

struct Turn
{
    Character * Character;

    Integer Initiative;

    bool operator > (const Turn &other)
    {
        return Initiative > other.Initiative;
    }
};

class BattleScene : public Scene 
{
    friend class BattleInterface;

    friend class BattleTileModel;

    friend class BattleState;

    BattleMap * battleMap;

    BattleInterface * battleInterface;

    BattleTileModel * battleTileModel;

    Character * selectedCharacter;

    BattleTile * hoveredTile;

    Array <Group> groups;

    Array <Turn> turnOrder;

    Turn * turn;

    CharacterActionData lastActionData;

    static BattleScene* instance;

    void Initialize() override;

    void Render() override;

    void Update() override;

    void CheckTileSelection();

    void DetermineTurnOrder();

    void CheckCharacterMovement();

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

    BattleScene();

    void SelectCharacter(Character*);

    void TargetCharacter(Character*);

    Character* GetSelectedCharacter() const {return selectedCharacter;}

    BattleMap* GetBattleMap() {return battleMap;}

    Turn* GetTurn() {return turn;}

    bool IsCharactersTurn(Character*) const;

    const CharacterActionData & GetLastAction() const {return lastActionData;}

    static BattleScene* Get() 
    {
        if(instance == nullptr)
        {
            instance = new BattleScene();
        }
        
        return instance;
    }
};