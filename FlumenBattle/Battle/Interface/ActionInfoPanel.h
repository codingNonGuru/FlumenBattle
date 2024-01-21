#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class ResourceCounter;
}

namespace battle::interface
{
    class ActionInfoPanel : public Element
    {
        static constexpr auto LABEL_DISTANCE = 10.0f;

        Element *border;

        Array <Text*> actionLabels;

        Array <Text*> subactionLabels;

        LayoutGroup * actionLayoutGroup;

        LayoutGroup * subactionLayoutGroup;

        world::interface::ResourceCounter *actionCounter;

        world::interface::ResourceCounter *movementCounter;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleCharacterSelected();

        void RefreshWeapons();

        void RefreshSpells();

        void DisableSubactionLabels();

        void HandleActionSelected();

        void HandleSubactionSelected();

        void HandleWeaponSelected();

        void HandleSpellSelected();
    };
}