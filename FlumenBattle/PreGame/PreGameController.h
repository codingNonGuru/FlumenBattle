#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenCore/Singleton.h"

namespace pregame
{
    class PreGameController : public core::Singleton<PreGameController>
    {
        friend class PreGameState;

        void Enable();

        void Disable();
    };
}