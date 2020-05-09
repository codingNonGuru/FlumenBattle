#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world 
{
    namespace group
    {
        class Group;
    }
}

namespace world
{
    class Battle
    {
        bool isActive;

        group::Group *first;

        group::Group *second;

    public:
        Battle() {}

        Battle(group::Group *, group::Group *);

        void Update();

        group::Group * GetFirst() const {return first;}

        group::Group * GetSecond() const {return second;}

        bool IsActive() const {return isActive;}

        void Finish() {isActive = false;}
    };
}