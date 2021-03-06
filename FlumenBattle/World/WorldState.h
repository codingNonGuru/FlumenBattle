#pragma once

#include "FlumenEngine/Core/State.hpp"

class WorldInterface;

namespace world
{
    class WorldScene;
    class WorldController;
    
    class WorldState : public State
    {
        WorldScene *scene;

        WorldInterface *interface;

        WorldController *controller;

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
}