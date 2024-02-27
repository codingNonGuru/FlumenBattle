#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::character
{
    struct RecruitData;
}

namespace world::interface
{
    class ResourceCounter;
    class RecruitmentMenu;

    class RecruitmentItem : public Element
    {
        RecruitmentMenu *parent;

        Text *classLabel;

        Text *raceLabel;

        interface::ResourceCounter *costCounter;

        Text *recruitLabel;

        Text *levelLabel;

        Element *icon;

        const character::RecruitData *recruitData;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

    public:
        void Setup(RecruitmentMenu *, const character::RecruitData *);
    };
}