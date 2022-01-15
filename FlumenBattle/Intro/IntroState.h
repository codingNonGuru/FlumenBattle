#pragma once

#include "FlumenEngine/Core/State.hpp"
#include "FlumenEngine/Core/Singleton.h"

namespace intro
{
    //class PreGameController;

    class IntroState : public State, public Singleton<IntroState>
    {
        //PreGameController *controller;

        void HandleEnter() override;

        void HandleExit() override;
    };
}