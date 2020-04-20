#pragma once

#include "FlumenEngine/Core/State.hpp"

class WorldScene;

class WorldState : public State
{
    WorldScene *scene;

    WorldState();

    void Initialize();

    void HandleEnter() override;

    void HandleExit() override;

public:
    static WorldState * Get()
    {
        static WorldState state;

        return &state;
    }
};