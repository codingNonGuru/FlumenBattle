#pragma once

#include "FlumenCore/Conventions.hpp"

class Group;
class Troop;

struct CharacterBattleData
{
    class Character * Character;

    Position2 Position;

    Rotation Direction;

    CharacterBattleData * Target;
};

struct GroupBattleData
{
    class Group * Group;

    container::Block <CharacterBattleData, 16> TroopDatas;

    GroupBattleData() {}

    GroupBattleData(class Group *);
};

class Battle
{
    GroupBattleData first;

    GroupBattleData second;

public:
    Battle() {}

    Battle(Group *, Group *);

    void Update();
};