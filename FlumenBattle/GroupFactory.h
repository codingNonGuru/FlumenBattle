#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;
class Group;
struct BattleTile;

struct GroupBuildData
{
    BattleTile * Tile;
};

class GroupFactory
{
public:
    static Group* Create(Array <Group> &, GroupBuildData); 
};