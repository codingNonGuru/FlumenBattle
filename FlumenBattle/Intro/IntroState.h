#pragma once

#include "FlumenEngine/Core/State.hpp"
#include "FlumenCore/Singleton.h"

namespace intro
{
    //class PreGameController;

    class IntroState : public State, public core::Singleton<IntroState>
    {
        //PreGameController *controller;

        void HandleEnter() override;

        void HandleExit() override;
    };
}