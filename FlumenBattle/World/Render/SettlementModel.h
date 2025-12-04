#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class SettlementModel : public core::Singleton <SettlementModel>
    {
    public:
        void Initialize();

        void Render();
    };
}