#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Render/Model.hpp"

#include "FlumenBattle/World/Render/FarmData.h"

class Camera;

namespace world::render
{
    class FarmModel : public Model, public core::Singleton <FarmModel>
    {
        friend class RendererAllocator;

        container::Array <Position2> positions;

        container::Array <Position2> textureOffsets;

        container::Array <Float> rotations;

        container::Grid <FarmData> farmData;

        Camera* camera;

        void TransferData();

    public:
        void Initialize() override;

        void Render();
    };
}