#pragma once

#include "FlumenCore/Singleton.h"

namespace world::sound
{
    class WorldMixer : public core::Singleton <WorldMixer>
    {
        friend class core::Singleton <WorldMixer>;

        WorldMixer();
    };
}