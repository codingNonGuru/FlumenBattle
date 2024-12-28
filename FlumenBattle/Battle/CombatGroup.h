#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::group
{
    class GroupCore;
}

namespace battle
{
    class Combatant;
    struct BattleTile;

    class CombatGroup
    {
        friend class BattleScene;

        world::group::GroupCore *group;

        Array <Combatant> combatants;

        BattleTile *tile;

    public:
        CombatGroup();

        void Initialize(class world::group::GroupCore *, BattleTile *);

        world::group::GroupCore *GetGroup() const {return group;}

        const Array <Combatant> &GetCombatants() const {return combatants;}

        bool IsAlive() const;
    };
}