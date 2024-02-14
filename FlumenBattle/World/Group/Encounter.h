#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world 
{
    namespace group
    {
        class Group;
    }
}

namespace world::group
{
    class Encounter
    {
        bool isOngoing;

        bool isBattle;

        bool hasBattleEnded;

        Group *first;

        Group *second;

        Group *winner;

    public:
        Encounter() {}

        void Initialize(Group *, Group *);

        void Update();

        void StartFighting() {isBattle = true;}

        void EndFighting() {isBattle = false; hasBattleEnded = true;}

        group::Group *GetFirst() const {return first;}

        group::Group *GetSecond() const {return second;}

        group::Group *GetOtherThan(group::Group *group) const {return group == first ? second : first;}

        bool IsOngoing() const {return isOngoing;}

        bool IsBattle() const {return isBattle;}

        bool HasBattleEnded() const {return hasBattleEnded;}

        bool IsWinner(Group *group) const {return group == winner;}

        void Finish(Group *);
    };
}