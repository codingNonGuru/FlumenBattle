#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Animation;

namespace world::interface
{
    class MoneyPopup : public Element
    {
        LayoutGroup *backdrop;

        Text *label;

        Element *coinIcon;

        Animation *animation;

        Position2 followedPosition;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        void Setup(int);
    };
}