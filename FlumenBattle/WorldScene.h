#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class Group;
class Battle;

class WorldScene : Scene
{
    friend class WorldState;

    Battle *battle;

    Array <Group> groups;

    void Initialize() override;

    void HandleEnable() override;

    void Update() override;

    void Render() override;

    void HandleEnterBattle();

public:
    static WorldScene * Get() 
    {
        static WorldScene scene;

        return &scene;
    }

    Battle * GetBattle() const {return battle;}
};