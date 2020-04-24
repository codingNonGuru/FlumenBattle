#pragma once

#include "FlumenEngine/Core/State.hpp"

class WorldScene;
class WorldInterface;

class WorldState : public State
{
    WorldScene *scene;

    WorldInterface *interface;

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