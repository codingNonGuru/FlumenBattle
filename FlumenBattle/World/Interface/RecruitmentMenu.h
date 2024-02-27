#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::character
{
    struct RecruitData;
}

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class RecruitmentItem;

    class RecruitmentMenu : public Element
    {
        Element *border;

        Text *titleLabel;

        container::Array <RecruitmentItem *> items;

        LayoutGroup *itemLayout;

        settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleSetupAnimations() override;

        void HandleClosePressed();

        void HandlePoolUpdate();

        void RefreshItemList();

    public:
        void Setup(settlement::Settlement *);

        void ProcessInput(const character::RecruitData *);
    };
}