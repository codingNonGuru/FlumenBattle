#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Animation;
class LayoutGroup;

namespace utility
{
    class Success;
}

namespace world::interface
{
    class RollPopup : public Element
    {
        Text *label;

        Element *diceIcon;

        LayoutGroup *backdrop;

        Animation *animation;

        Position2 followedPosition;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        void Setup(utility::Success);
    };
}