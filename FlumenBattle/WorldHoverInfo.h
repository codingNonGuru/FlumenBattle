#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class ProgressBar;

namespace world
{
    class WorldHoverInfo : public Element
    {
        Text *infoLabel;

        Text *fpsLabel;

        ProgressBar *scienceProgress;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void DisplayWorldInfo();
    };
}