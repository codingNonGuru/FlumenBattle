#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class CharacterModel;
class Group;
class CharacterInfo;
class Character;
class BattleTileModel;
class BattleMap;

class BattleScene : public Scene 
{
    friend class CharacterModel;

    friend class BattleTileModel;

    BattleMap *battleMap;

    CharacterModel* model;

    BattleTileModel* battleTileModel;

    Character* selected;

    Array <Group> groups;

    void Initialize() override;

    void Render() override;

public:
    BattleScene();

    void SelectCharacter(Character*);

    void TargetCharacter(Character*);

    BattleMap* GetBattleMap() {return battleMap;}
};