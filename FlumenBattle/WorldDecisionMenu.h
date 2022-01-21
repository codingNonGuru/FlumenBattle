#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    class WorldDecisionMenu : public Element
    {
        Text *travelLabel;
        
        Text *searchLabel;

        Text *restLabel;

        Text *statusLabel;

        Array <Text *> rollLabels;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleActionSelected();
    };
}