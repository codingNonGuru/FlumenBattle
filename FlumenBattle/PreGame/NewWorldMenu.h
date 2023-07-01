#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace pregame
{
    class NewWorldMenu : public Element
    {
        friend class PreGameState;

        int sizeParameterValue;

        Text *sizeLabel;

        Text *generateLabel;

        Text *backLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnGenerateWorldPressed();

        void OnBackPressed();

        void OnSizeIncreasePressed();

        void OnSizeDecreasePressed();
    };
}