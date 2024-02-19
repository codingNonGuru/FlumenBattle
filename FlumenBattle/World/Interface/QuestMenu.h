#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class QuestMenuItem;

    class QuestMenu : public Element
    {
        Element *border;

        Text *titleLabel;

        container::Array <QuestMenuItem *> items;

        LayoutGroup *itemLayout;

        void HandleConfigure() override;

        void HandleUpdate() override;
    };
}