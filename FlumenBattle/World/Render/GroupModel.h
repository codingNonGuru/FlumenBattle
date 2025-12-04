#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class GroupModel : public core::Singleton <GroupModel>
    {
    public:
        void Initialize();

        void Render();

        void RenderSightings();
    };
}