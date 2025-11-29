#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class RiverModel : public core::Singleton <RiverModel>
    {
    public:
        void Render();
    };
}