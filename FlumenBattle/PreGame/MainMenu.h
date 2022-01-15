#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class MainMenu : public Element
    {
        friend class PreGameState;

        Text *startGameLabel;

        Text *quitGameLabel;

        void HandleConfigure() override;
    };
}