#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class ProgresBar;

namespace world
{
    class WorldDecisionMenu : public Element
    {
        Element *border;

        Text *statusLabel;

        Text *timeLabel;

        Array <Text *> rollLabels;

        ProgressBar *actionProgress;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleActionSelected();
    };
}