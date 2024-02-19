#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Animation;

namespace world::group
{
    enum class GroupActions;
}

namespace world::interface
{
    class ActionPopup : public Element
    {
        Text *label;

        Element *backdrop;

        Animation *animation;

        Position2 followedPosition;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        void Setup(group::GroupActions, bool);
    };
}