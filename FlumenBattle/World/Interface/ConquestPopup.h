#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class ConquestPopup : public Element
    {
        Element *border;

        Text *infoLabel;

        Text *continueLabel;

        void HandleConfigure() override;

        void HandleSetupAnimations() override;

        void HandleClosePressed();

    public:
        void Setup(settlement::Settlement *);
    };
}