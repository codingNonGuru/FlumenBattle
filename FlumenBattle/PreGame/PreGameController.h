#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/Singleton.h"

namespace pregame
{
    class PreGameController : public Singleton<PreGameController>
    {
        friend class PreGameState;

        void Enable();

        void Disable();
    };
}