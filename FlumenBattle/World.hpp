#pragma once

class Group;
class Battle;

class World
{
    Group * caravan;

    Group * bandits;

    Battle * battle;

public:
    World();

    void Update();
};