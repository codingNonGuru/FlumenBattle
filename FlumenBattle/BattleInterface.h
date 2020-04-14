#pragma once

#include "FlumenCore/Conventions.hpp"

class BattleScene;
class Character;
class CharacterInfo;

class BattleInterface 
{
    BattleScene* battleScene;

    Array <CharacterInfo*> characterInfos;

public:
    void Initialize();
};