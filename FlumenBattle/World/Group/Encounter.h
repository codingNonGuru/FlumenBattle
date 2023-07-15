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

    public:
        Encounter() {}

        void Initialize(Group *, Group *);

        void Update();

        void StartFighting() {isBattle = true;}

        void EndFighting() {isBattle = false; hasBattleEnded = true;}

        group::Group * GetFirst() const {return first;}

        group::Group * GetSecond() const {return second;}

        bool IsOngoing() const {return isOngoing;}

        bool IsBattle() const {return isBattle;}

        bool HasBattleEnded() const {return hasBattleEnded;}

        void Finish();
    };
}