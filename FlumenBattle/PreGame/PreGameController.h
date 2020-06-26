#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace pregame
{
    class PreGameController : public Singleton<PreGameController>
    {
        friend class PreGameState;

        void Enable();

        void Disable();

        void HandleStartGamePressed();
    };
}