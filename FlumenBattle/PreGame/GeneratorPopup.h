#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class GeneratorPopup : public Element
    {
        Text *loadingLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;
    };
}