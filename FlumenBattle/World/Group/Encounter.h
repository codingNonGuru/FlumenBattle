#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world 
{
    namespace group
    {
        class GroupCore;
    }

    class WorldTile;
}

namespace world::group
{
    class Encounter
    {
        bool isOngoing;

        bool isBattle;

        bool isSiege;

        bool hasBattleEnded;

        bool isPlayerInvolved;

        GroupCore *attacker;

        GroupCore *defender;

        GroupCore *winner;

        WorldTile *location;

    public:
        Encounter() {}

        void Initialize(GroupCore *, GroupCore *);

        void SetSiege(bool);

        void Update();

        void StartFighting() {isBattle = true;}

        void EndFighting() {isBattle = false; hasBattleEnded = true;}

        group::GroupCore *GetAttacker() const {return attacker;}

        group::GroupCore *GetDefender() const {return defender;}

        group::GroupCore *GetOtherThan(group::GroupCore *group) const {return group == attacker ? defender : attacker;}

        bool IsOngoing() const {return isOngoing;}

        bool IsBattle() const {return isBattle;}

        bool IsSiege() const {return isSiege;}

        bool HasBattleEnded() const {return hasBattleEnded;}

        bool IsWinner(GroupCore *group) const {return group == winner;}

        void Finish(GroupCore *);

        void Terminate();
    };
}