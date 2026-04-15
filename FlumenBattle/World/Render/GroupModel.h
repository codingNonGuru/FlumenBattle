#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class GroupModel : public core::Singleton <GroupModel>
    {
    public:
        void Initialize();

        void PrepareData();

        void Render();

        void RenderShadows();

        void RenderSightings();
    };
}