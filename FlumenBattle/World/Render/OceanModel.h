#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    struct FoamSegmentData;

    class OceanModel : public core::Singleton <OceanModel>
    {
    public:
        engine::render::LineRenderer *GetFoamRenderer();

        void SetFoamRenderer(engine::render::LineRenderer *);

        container::Array <FoamSegmentData> &GetFoamSegments();

        void Initialize();

        void Render();
    };
}