#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Types.h"

class Text;
class Animation;

namespace utility
{
    class Success;
}

namespace world::interface::popup
{
    class GenericPopup : public Element
    {
    protected:
        PopupTypes popupType;

        Element *backdrop;

        Text *label;

        Element *icon;

        Animation *animation;

        Position2 followedPosition;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        virtual void Setup() = 0;
    };
}