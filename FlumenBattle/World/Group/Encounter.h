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

        Group *first;

        Group *second;

    public:
        Encounter() {}

        void Initialize(Group *, Group *);

        void Update();

        group::Group * GetFirst() const {return first;}

        group::Group * GetSecond() const {return second;}

        bool IsOngoing() const {return isOngoing;}

        void Finish();
    };
}