#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class NewGameMenu : public Element
    {
        friend class PreGameState;

        Text *newWorldLabel;

        Text *loadWorldLabel;

        Text *backLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnNewWorldPressed();

        void OnBackPressed();
    };
}