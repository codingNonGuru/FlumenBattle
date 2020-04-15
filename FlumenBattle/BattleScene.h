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

    friend class BattleController;

    BattleMap * battleMap;

    BattleInterface * battleInterface;

    BattleTileModel * battleTileModel;

    Array <Group> groups;

    Array <Turn> turnOrder;

    Turn * turn;

    static BattleScene* instance;

    void Initialize() override;

    void HandleEnable() override;

    void Render() override;

    void Update() override;

    void DetermineTurnOrder();

public:
    Delegate OnInitialized;

    Delegate OnEnabled;

    BattleScene();

    BattleMap* GetBattleMap() {return battleMap;}

    Turn* GetTurn() {return turn;}

    Character * GetActingCharacter() {return turn->Character;}

    Camera * GetCamera() const;

    Group * GetRandomGroup();

    bool IsCharactersTurn(Character*) const;

    void EndTurn();

    static BattleScene* Get(); 
};