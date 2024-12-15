#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world
{
    class GroupEngageMenu : public Element
    {
        Element *border;

        LayoutGroup *mainLayout;
        
        Text *descriptionLabel;

        Text *lootLabel;

        Text *resultLabel;

        Element *coinIcon;

        LayoutGroup *optionLayout;

        container::Array <Text *> optionLabels;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleFightPressed();

        void HandleSneakPressed();

        void HandlePersuadePressed();

        void HandleBribePressed();

        void HandleLeavePressed();

        void HandleConquerPressed();

        void RefreshOptions();

        void DisableInput();
    };
}