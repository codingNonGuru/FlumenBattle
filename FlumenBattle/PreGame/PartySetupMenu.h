#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class PartySetupMenu : public Element
    {
        friend class PreGameState;

        Text *titleLabel;

        Text *startGameLabel;

        Text *loadPresetLabel;

        Text *savePresetLabel;

        Text *backLabel;

        Element *border;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnStartGamePressed();

        void OnBackPressed();
    };
}