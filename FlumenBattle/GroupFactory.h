#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

class Character;
class Group;
struct BattleTile;

struct GroupBuildData
{
    RaceTypes Race;
};

class GroupFactory
{
public:
    static Group* Create(Array <Group> &, GroupBuildData); 
};