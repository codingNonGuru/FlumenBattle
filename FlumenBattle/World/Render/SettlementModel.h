#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Render/SettlementRenderData.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::render
{
    struct PopulationData
    {
        const world::settlement::Settlement *Settlement;

        int Population;
    };

    class SettlementModel : public core::Singleton <SettlementModel>
    {   
        friend class RendererAllocator;

        container::Array <BuildingRenderData> buildingData;

        container::Array <unsigned int> settlementIndices;

        DataBuffer *buildingDataBuffer;

        DataBuffer *indexBuffer;

        container::Pool <PopulationData> populationTracker;

        void UpdateIndices();

        void CheckPopulation();

        void HandleSettlementFounded();

    public:
        void Initialize();

        void Render();

        void RenderShadows();

        static const int MAX_BUILDINGS_PER_SETTLEMENT = 64;
    };
}