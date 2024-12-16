#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Types.h"

class Text;
class SimpleList;

namespace world::interface
{
    struct SideButtonData
    {
        MajorMenus Type;

        Element *Menu;
    };

    class SideButton : public Element
    {
        Element *frame;

        Text *label;

        Element *menu;

        MajorMenus type;

        float offsetFactor {0.0f};

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(MajorMenus, Element *);
    };

    class SideButtonSet : public Element
    {
        container::Array <SideButton *> buttons;

        SimpleList *buttonList;

        void HandleConfigure() override;

    public:
        void Setup(const std::initializer_list <SideButtonData>);
    };
}