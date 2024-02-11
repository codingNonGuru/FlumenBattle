#pragma once

#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/State.hpp"

namespace world
{
    class WorldScene;
    class WorldController;
    class WorldInterface;

    namespace render
    {
        class WorldTileModel;
    }
    
    class WorldState : public State
    {
        WorldScene *scene;

        WorldInterface *interface;

        WorldController *controller;

        render::WorldTileModel *tileModel;

        WorldState();

        void Initialize();

        void HandleEnter() override;

        void HandleExit() override;

        void Update() override;

    public:
        Delegate OnUpdate;

        static WorldState * Get()
        {
            static WorldState state;

            return &state;
        }
    };
}