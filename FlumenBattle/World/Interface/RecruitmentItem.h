#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class RecruitmentMenu;

    class RecruitmentItem : public Element
    {
        RecruitmentMenu *parent;

        Text *recruitLabel;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

    public:
        void Setup(RecruitmentMenu *);
    };
}