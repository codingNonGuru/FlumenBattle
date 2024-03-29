#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class MainMenu : public Element
    {
        friend class PreGameState;

        Text *newGameLabel;

        Text *quitGameLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleNewGamePressed();

        void OnExitPressed();
    };
}