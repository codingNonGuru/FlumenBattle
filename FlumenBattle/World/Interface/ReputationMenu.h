#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class ReputationMenu : public Element
    {
        Element *border;

        Text *titleLabel;

        container::Array <Text *> items;

        LayoutGroup *itemLayout;

        void HandleConfigure() override;

        void HandleUpdate() override;
    };
}