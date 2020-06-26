#pragma once

#include "FlumenEngine/Core/State.hpp"
#include "FlumenEngine/Core/Singleton.h"

namespace pregame
{
    class PreGameController;

    class PreGameState : public State, public Singleton<PreGameState>
    {
        PreGameController *controller;

        void HandleEnter() override;

        void HandleExit() override;
    };
}