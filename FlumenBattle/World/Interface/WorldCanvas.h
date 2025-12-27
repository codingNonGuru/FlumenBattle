#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Scroller;

namespace world::interface
{
    class WorldCanvas : public Element
    {
        void HandleConfigure() override;

        void HandleScrollUp();

        void HandleScrollDown();
    };
}