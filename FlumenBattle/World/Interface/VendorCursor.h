#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world::interface
{
    class VendorCursor : public Element
    {   
        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;
    };
}