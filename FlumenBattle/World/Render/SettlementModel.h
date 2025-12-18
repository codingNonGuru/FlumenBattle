#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Render/SettlementRenderData.h"

namespace world::render
{
    class SettlementModel : public core::Singleton <SettlementModel>
    {
        container::Array <BuildingRenderData> buildingData;

        container::Array <unsigned int> settlementIndices;

        DataBuffer *buildingDataBuffer;

        DataBuffer *indexBuffer;

    public:
        void Initialize();

        void Render();

        void RenderShadows();
    };
}