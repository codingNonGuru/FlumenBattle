#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::group
{
    class Group;
}
class Combatant;
struct BattleTile;

class CombatGroup
{
    friend class BattleScene;

    world::group::Group *group;

    Array <Combatant> combatants;

    BattleTile *tile;

public:
    CombatGroup();

    void Initialize(class world::group::Group *, BattleTile *);

    world::group::Group *GetGroup() const {return group;}

    const Array <Combatant> &GetCombatants() const {return combatants;}

    bool IsAlive() const;
};