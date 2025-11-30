#include "FlumenEngine/Render/LineRenderer.h"

#include "RendererAllocator.h"
#include "FlumenBattle/World/Render/FarmModel.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Render/OceanModel.h"
#include "FlumenBattle/World/Render/FoamSegmentData.h"

using namespace world::render;

RendererAllocator::RendererAllocator()
{
    static const auto MAXIMUM_WORLD_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WORLD_SIZE).Integer;

    auto width = MAXIMUM_WORLD_SIZE;

    auto height = MAXIMUM_WORLD_SIZE;

    farmPositionMemory = container::Array <Position2>::PreallocateMemory(width * height);

    farmOffsetMemory = container::Array <Position2>::PreallocateMemory(width * height);

    farmRotationMemory = container::Array <Float>::PreallocateMemory(width * height);

    farmDataMemory = container::Grid <FarmData>::PreallocateMemory(width * height);


    auto maximumSegmentCount = width * height / 8;

    foamSegmentMemory = container::Array <FoamSegmentData>::PreallocateMemory(maximumSegmentCount);

    OceanModel::Get()->SetFoamRenderer(new engine::render::LineRenderer(maximumSegmentCount));
}

void RendererAllocator::Allocate(int size)
{
    FarmModel::Get()->positions.Initialize(size * size, farmPositionMemory);

    FarmModel::Get()->textureOffsets.Initialize(size * size, farmOffsetMemory);

    FarmModel::Get()->rotations.Initialize(size * size, farmRotationMemory);

    FarmModel::Get()->farmData.Initialize(size, size, farmDataMemory);

    OceanModel::Get()->GetFoamSegments().Initialize(size * size / 8, foamSegmentMemory);
}