#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class OceanModel : public core::Singleton <OceanModel>
    {
    public:
        void Initialize();

        void Render();
    };
}