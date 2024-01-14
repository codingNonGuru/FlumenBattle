#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    class GroupEngageMenu : public Element
    {
        Text *descriptionLabel;

        container::Array <Text *> optionLabels;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleFightPressed();

        void HandlePersuadePressed();

        void HandleLeavePressed();

        void RefreshOptions();

        void DisableInput();
    };
}