#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class GeneratedWorldMenu : public Element
    {
        friend class PreGameState;

        Element *backdrop;

        Text *sizeLabel;

        Text *startGameLabel;

        Text *changePartyLabel;

        Text *saveWorldLabel;

        Text *backLabel;

        Element *border;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnStartGamePressed();

        void OnChangePartyPressed();

        void OnBackPressed();
    };
}