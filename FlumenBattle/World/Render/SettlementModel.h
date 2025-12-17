#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Render/SettlementRenderData.h"

namespace world::render
{
    class SettlementModel : public core::Singleton <SettlementModel>
    {
        container::Array <BuildingRenderData> buildingData;

        DataBuffer *buildingDataBuffer;

    public:
        void Initialize();

        void Render();
    };
}