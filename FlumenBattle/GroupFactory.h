#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;
class Group;

class GroupFactory
{
public:
    static Group* Create(Array <Group> &); 
};