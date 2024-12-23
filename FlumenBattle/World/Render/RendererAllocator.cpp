#include "RendererAllocator.h"
#include "FlumenBattle/World/Render/FarmModel.h"
#include "FlumenBattle/Config.h"

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
}

void RendererAllocator::Allocate(int size)
{
    FarmModel::Get()->positions.Initialize(size * size, farmPositionMemory);

    FarmModel::Get()->textureOffsets.Initialize(size * size, farmOffsetMemory);

    FarmModel::Get()->rotations.Initialize(size * size, farmRotationMemory);

    FarmModel::Get()->farmData.Initialize(size, size, farmDataMemory);
}