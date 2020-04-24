#pragma once

#include "FlumenCore/Conventions.hpp"

class Group;
class Combatant;
struct BattleTile;

class CombatGroup
{
    friend class BattleScene;

    Group *group;

    Array <Combatant> combatants;

    BattleTile *tile;

public:
    CombatGroup();

    void Initialize(class Group *, BattleTile *);

    Group *GetGroup() const {return group;}

    const Array <Combatant> &GetCombatants() const {return combatants;}

    bool IsAlive() const;
};