#pragma once

#include "FlumenEngine/Core/State.hpp"

class Element;

namespace pregame
{
    class PreGameController;
    class MainMenu;

    class PreGameState : public State
    {
        PreGameController *controller;

        Element *canvas;

        MainMenu *mainMenu;

        PreGameState();

        void HandleEnter() override;

        void HandleExit() override;

    public:
        static PreGameState * Get()
        {
            static PreGameState state;

            return &state;
        }
    };
}