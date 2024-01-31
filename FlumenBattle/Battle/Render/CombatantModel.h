#pragma once

#include "FlumenCore/Singleton.h"

class Camera;

namespace battle::render
{
    class CombatantModel : public core::Singleton <CombatantModel>
    {
        Camera *camera;

    public:
        void Initialize();

        void Render();
    };
}