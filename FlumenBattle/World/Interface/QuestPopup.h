#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::group
{
    struct Quest;
}

namespace world::interface
{
    class QuestPopup : public Element
    {
        Element *border;

        Text *infoLabel;

        Text *continueLabel;

        void HandleConfigure() override;

        void HandleClosePressed();

    public:
        void Setup(const group::Quest &, bool);
    };
}