#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Animation;
class LayoutGroup;

namespace world::group
{
    struct ItemChange;
}

namespace world::interface
{
    class ItemPopup : public Element
    {
        LayoutGroup *backdrop;

        Text *label;

        Element *itemIcon;

        Animation *animation;

        Position2 followedPosition;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        void Setup(const group::ItemChange &);
    };
}