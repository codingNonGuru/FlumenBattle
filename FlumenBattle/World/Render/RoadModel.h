#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class RoadModel : public core::Singleton <RoadModel>
    {
    public:
        void Initialize();

        void Render();
    };
}