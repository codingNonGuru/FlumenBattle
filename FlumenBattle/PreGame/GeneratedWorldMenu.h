#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class GeneratedWorldMenu : public Element
    {
        friend class PreGameState;

        Text *startGameLabel;

        Text *saveWorldLabel;

        Text *backLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnStartGamePressed();

        void OnBackPressed();
    };
}