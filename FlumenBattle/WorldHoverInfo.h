#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    class WorldHoverInfo : public Element
    {
        Text * strengthLabel;

        Text * diseaseLabel;

        Text * industryLabel;

        void HandleConfigure() override;

        void HandleUpdate() override;
    };
}