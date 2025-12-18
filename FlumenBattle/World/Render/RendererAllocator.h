#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Render/FarmData.h"
#include "FlumenBattle/World/Render/RiverRenderData.h"
#include "FlumenBattle/World/Render/SettlementRenderData.h"

namespace world::render
{
    struct FoamSegmentData;

    struct RoadRenderData;

    struct PathTileData;

    struct PopulationData;

    class RendererAllocator : public core::Singleton <RendererAllocator>
    {
        friend class core::Singleton <RendererAllocator>;

        container::Array <Position2>::Memory farmPositionMemory;

        container::Array <Position2>::Memory farmOffsetMemory;

        container::Array <Float>::Memory farmRotationMemory;

        container::Grid <FarmData>::Memory farmDataMemory;


        container::Array <FoamSegmentData>::Memory foamSegmentMemory;


        container::Array <Position2>::Memory treePositionMemory;

        container::Array <Float4>::Memory treeColorMemory;

        container::Array <Float>::Memory treeScaleMemory;

        container::Array <unsigned int>::Memory treeQueueMemory;


        container::Array <Position2>::Memory mountainPositionMemory;

        container::Array <Float4>::Memory mountainColorMemory;

        container::Array <Float>::Memory mountainScaleMemory;

        container::Array <unsigned int>::Memory mountainQueueMemory;

        container::Array <int>::Memory mountainTypeMemory;


        container::Array <RoadRenderData>::Memory roadDataMemory;

        container::Array <unsigned int>::Memory roadQueueMemory;

        container::Grid <PathTileData>::Memory roadTileDataMemory;


        container::Array <RiverRenderData>::Memory riverDataMemory;


        container::Array <BuildingRenderData>::Memory buildingDataMemory;

        container::Array <unsigned int>::Memory settlementIndexMemory;

        container::Pool <PopulationData>::Memory populationDataMemory;

        RendererAllocator();

    public:
        void Allocate(int size);
    };
}