#pragma once

#include "FlumenCore/Conventions.hpp"

class Group;

class Battle
{
    Group *first;

    Group *second;

public:
    Battle() {}

    Battle(Group *, Group *);

    void Update();

    Group * GetFirst() const {return first;}

    Group * GetSecond() const {return second;}
};