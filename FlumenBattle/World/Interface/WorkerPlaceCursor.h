#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world::interface
{
    class Counter;

    class WorkerPlaceCursor : public Element
    {   
        Counter *counter;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;
    };
}