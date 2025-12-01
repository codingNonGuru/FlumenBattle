#pragma once

#include "FlumenCore/Singleton.h"

namespace world::render
{
    class TreeModel : public core::Singleton <TreeModel>
    {
        void PrepareQueue();

    public:
        void Initialize();

        void RenderShadows();

        void Render();
    };
}