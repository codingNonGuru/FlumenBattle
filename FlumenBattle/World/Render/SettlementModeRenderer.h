#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class SettlementModeRenderer : public core::Singleton <SettlementModeRenderer>
    {
    public:
        SettlementModeRenderer();
        
        void RenderBorderExpansionMap();

        void RenderSettleModeMap();
        
        void RenderExploreMap();
        
        void RenderTileDevelopMap();
    };
}