#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace battle::interface
{
    class TargetCursor : public Element
    {   
        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;
    };
}