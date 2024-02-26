#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class RecruitmentItem;

    class RecruitmentMenu : public Element
    {
        Element *border;

        Text *titleLabel;

        container::Array <RecruitmentItem *> items;

        LayoutGroup *itemLayout;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleClosePressed();

    public:
        void ProcessInput();
    };
}