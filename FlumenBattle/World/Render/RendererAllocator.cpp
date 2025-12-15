#include "FlumenEngine/Render/LineRenderer.h"
#include "FlumenEngine/Render/DataBuffer.hpp"

#include "RendererAllocator.h"
#include "FlumenBattle/World/Render/FarmModel.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Render/OceanModel.h"
#include "FlumenBattle/World/Render/FoamSegmentData.h"
#include "FlumenBattle/World/Render/TreeModel.h"
#include "FlumenBattle/World/Render/MountainRenderer.h"
#include "FlumenBattle/World/Render/RoadModel.h"

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


    auto maximumTreeCount = TreeModel::GetMaximumTreesPerTile() * width * height;

    treePositionMemory = container::Array <Position2>::PreallocateMemory(maximumTreeCount);

    treeColorMemory = container::Array <Float4>::PreallocateMemory(maximumTreeCount);

    treeScaleMemory = container::Array <Float>::PreallocateMemory(maximumTreeCount);

    treeQueueMemory = container::Array <unsigned int>::PreallocateMemory(width * height);


    auto maximumMountainCount = MountainRenderer::GetMaximumMountainsPerTile() * width * height;

    mountainPositionMemory = container::Array <Position2>::PreallocateMemory(maximumMountainCount);

    mountainColorMemory = container::Array <Float4>::PreallocateMemory(maximumMountainCount);

    mountainScaleMemory = container::Array <Float>::PreallocateMemory(maximumMountainCount);

    mountainQueueMemory = container::Array <unsigned int>::PreallocateMemory(width * height);

    mountainTypeMemory = container::Array <int>::PreallocateMemory(maximumMountainCount);


    auto maximumRoadCount = width * height;

    roadDataMemory = container::Array <RoadRenderData>::PreallocateMemory(maximumRoadCount);

    roadQueueMemory = container::Array <unsigned int>::PreallocateMemory(maximumRoadCount);

    roadTileDataMemory = container::Grid <PathTileData>::PreallocateMemory(maximumRoadCount);
}

void RendererAllocator::Allocate(int size)
{
    FarmModel::Get()->positions.Initialize(size * size, farmPositionMemory);

    FarmModel::Get()->textureOffsets.Initialize(size * size, farmOffsetMemory);

    FarmModel::Get()->rotations.Initialize(size * size, farmRotationMemory);

    FarmModel::Get()->farmData.Initialize(size, size, farmDataMemory);


    OceanModel::Get()->GetFoamSegments().Initialize(size * size / 8, foamSegmentMemory);


    auto treeCount = TreeModel::GetMaximumTreesPerTile() * size * size;

    TreeModel::Get()->positions.Initialize(treeCount, treePositionMemory);

    TreeModel::Get()->colors.Initialize(treeCount, treeColorMemory);

    TreeModel::Get()->scales.Initialize(treeCount, treeScaleMemory);

    TreeModel::Get()->tileQueue.Initialize(size * size, treeQueueMemory);

    TreeModel::Get()->positionBuffer = new DataBuffer(TreeModel::Get()->positions.GetMemoryCapacity());

    TreeModel::Get()->colorBuffer = new DataBuffer(TreeModel::Get()->colors.GetMemoryCapacity());

    TreeModel::Get()->scaleBuffer = new DataBuffer(TreeModel::Get()->scales.GetMemoryCapacity());

    TreeModel::Get()->tileQueueBuffer = new DataBuffer(TreeModel::Get()->tileQueue.GetMemoryCapacity());


    auto mountainCount = MountainRenderer::GetMaximumMountainsPerTile() * size * size;

    MountainRenderer::Get()->positions.Initialize(mountainCount, mountainPositionMemory);

    MountainRenderer::Get()->colors.Initialize(mountainCount, mountainColorMemory);

    MountainRenderer::Get()->scales.Initialize(mountainCount, mountainScaleMemory);

    MountainRenderer::Get()->tileQueue.Initialize(size * size, mountainQueueMemory);

    MountainRenderer::Get()->types.Initialize(mountainCount, mountainTypeMemory);

    MountainRenderer::Get()->positionBuffer = new DataBuffer(MountainRenderer::Get()->positions.GetMemoryCapacity());

    MountainRenderer::Get()->colorBuffer = new DataBuffer(MountainRenderer::Get()->colors.GetMemoryCapacity());

    MountainRenderer::Get()->scaleBuffer = new DataBuffer(MountainRenderer::Get()->scales.GetMemoryCapacity());

    MountainRenderer::Get()->tileQueueBuffer = new DataBuffer(MountainRenderer::Get()->tileQueue.GetMemoryCapacity());

    MountainRenderer::Get()->typeBuffer = new DataBuffer(MountainRenderer::Get()->types.GetMemoryCapacity());


    auto roadCount = size * size;

    RoadModel::Get()->data.Initialize(roadCount, roadDataMemory);

    RoadModel::Get()->tileQueue.Initialize(roadCount, roadQueueMemory);

    RoadModel::Get()->GetRoadTiles().Initialize(size, size, roadTileDataMemory);

    RoadModel::Get()->buffer = new DataBuffer(RoadModel::Get()->data.GetMemoryCapacity());

    RoadModel::Get()->tileQueueBuffer = new DataBuffer(RoadModel::Get()->tileQueue.GetMemoryCapacity());
}