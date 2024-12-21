#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class ImprovementCursor : public Element
    {   
        Element *backdrop;

        Text *label;

        Element *icon;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

        void HandleModeEnabled();
    };
}