#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class Group;
class Battle;

class WorldScene : Scene
{
    friend class WorldState;

    friend class WorldController;

    Integer hourCount;

    Battle *battle;

    Group *playerGroup;

    Array <Group> groups;

    void Initialize() override;

    void Render() override;

    void StartBattle();

    void Rest();

public:
    static WorldScene * Get() 
    {
        static WorldScene scene;

        return &scene;
    }

    Battle * GetBattle() const {return battle;}

    Group * GetPlayerGroup() const {return playerGroup;}

    Integer GetHourCount() const {return hourCount;}
};