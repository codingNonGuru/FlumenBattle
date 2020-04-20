#pragma once

#include "FlumenCore/Conventions.hpp"

class BattleScene;
class Character;
class CharacterInfo;
class BattleInfoPanel;
class CharacterDetailPanel;
class ActionInfoPanel;

class BattleInterface 
{
    BattleScene* battleScene;

    BattleInfoPanel *battleInfoPanel;

    CharacterDetailPanel *characterDetailPanel;

    ActionInfoPanel *actionInfoPanel;

    Array <CharacterInfo*> characterInfos;

public:
    BattleInterface();

    void Initialize();
};