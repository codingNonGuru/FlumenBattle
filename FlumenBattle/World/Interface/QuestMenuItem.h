#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::group
{
    struct Quest;
}

namespace world::interface
{
    class QuestMenuItem : public Element
    {
        Text *descriptionLabel;

        Text *extraInfoLabel;

        const group::Quest *quest;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const group::Quest *);
    };
}