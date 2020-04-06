#pragma once

#include "FlumenCore/Container/Block.hpp"
#include "FlumenCore/Conventions.hpp"

#include "BattleTypes.hpp"

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

    container::Block <CharacterBattleData, MAXIMUM_TROOP_COUNT> TroopDatas;

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